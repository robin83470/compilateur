#include "IRVisitor.h"
#include "IR.h"

namespace {
int parseCharLiteralValue(const std::string& text) {
    if (text[1] == '\\') {
        switch (text[2]) {
            case 'n':
                return '\n';
            case 't':
                return '\t';
            case '\\':
                return '\\';
            case '\'':
                return '\'';
            default:
                return text[2];
        }
    }
    return text[1];
}
}

IRVisitor::IRVisitor(SymbolTable* symbolTable)
    : symbolTable(symbolTable) {}

IRControlFlowGraph* IRVisitor::buildIr(antlr4::tree::ParseTree* tree) {
    currentCFG = new IRControlFlowGraph(symbolTable);
    currentCFG->addBasicBloc(".entry");

    // Réserver le temporaire pour la valeur de retour
    symbolTable->addSymbol("!retval");

    // Créer le bloc épilogue (il sera le dernier à être émis)
    epilogueBloc = currentCFG->addBasicBloc(".epilogue");

    // Revenir à .entry pour y mettre le code
    currentCFG->setCurrentBasicBloc(currentCFG->getBlocs()[0]);

    visit(tree);

    return currentCFG;
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext* ctx) {
    for (auto* func : ctx->function()) {
        visit(func);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitFunction(ifccParser::FunctionContext* ctx) {
    std::string funcName = ctx->ID() ? ctx->ID()->getText() : "main";

    // === CRÉER UNE NOUVELLE SYMBOL TABLE POUR CETTE FONCTION ===
    SymbolTable* oldSymbolTable = symbolTable;
    symbolTable = new SymbolTable();

    // === CRÉER UN NOUVEAU CFG AVEC LA NOUVELLE SYMBOL TABLE ===
    IRControlFlowGraph* oldCFG = currentCFG;
    currentCFG = new IRControlFlowGraph(symbolTable);

    // === INITIALISER LE CFG ===
    currentCFG->addBasicBloc(funcName + "_entry");
    currentCFG->setCurrentBasicBloc(currentCFG->getBlocs()[0]);

    symbolTable->addSymbol("!retval");
    epilogueBloc = currentCFG->addBasicBloc("." + funcName + "_exit");

    // === AJOUTER LES PARAMÈTRES ===
    if (ctx->paramList()) {
        auto* paramList = ctx->paramList();
        size_t numParams = paramList->ID().size();

        for (size_t i = 0; i < numParams; i++) {
            std::string paramName = paramList->ID(i)->getText();
            symbolTable->addSymbol(paramName);

            auto* bloc = currentCFG->getCurrentBasicBloc();
            bloc->addInstruction(new IRInstrGetParam(bloc, paramName, i));
        }
    }

    // === VISITER LE CORPS ===
    for (auto* stmt : ctx->stmt()) {
        visit(stmt);
    }
    allFunctions.push_back({currentCFG, symbolTable});
    // === RESTAURER L'ANCIENNE STATE ===
    symbolTable = oldSymbolTable;
    currentCFG = oldCFG;

    return 0;
}
IRBasicBloc* IRVisitor::createDeadBlock(const std::string& prefix) {
    return currentCFG->addBasicBlocUnique(prefix);
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) {
    std::string tmp = std::any_cast<std::string>(visit(ctx->rhs()));
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrCopy(bloc, "!retval", tmp));

    // Saut inconditionnel vers l'épilogue
    bloc->setExitTrue(epilogueBloc);

    // Créer un nouveau bloc pour le code mort après le return
    auto* deadBloc = createDeadBlock(".after_return");
    currentCFG->setCurrentBasicBloc(deadBloc);

    return 0;
}

antlrcpp::Any IRVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) {
    for (auto* decl : ctx->declarator()) {
        visit(decl);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitDeclarator(ifccParser::DeclaratorContext* ctx) {
    std::string varName = ctx->ID()->getText();

    int pointerDepth = 0;
    auto* p = ctx->pointer_prefix();
    while (p != nullptr) {
        pointerDepth++;
        p = p->pointer_prefix();
    }

    std::string declaredType = "int" + std::string(pointerDepth, '*');
    symbolTable->addSymbol(varName, declaredType);
    if (ctx->EQUAL()) {
        std::string tmp = std::any_cast<std::string>(visit(ctx->rhs()));
        auto* bloc = currentCFG->getCurrentBasicBloc();
        bloc->addInstruction(new IRInstrCopy(bloc, varName, tmp));
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitPointer_prefix(ifccParser::Pointer_prefixContext* ctx) {
    // The symbol table visitor already resolves declaration types.
    // IR generation does not need pointer depth at this stage.
    return 0;
}

antlrcpp::Any IRVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) {
    std::string lvalueResult = std::any_cast<std::string>(visit(ctx->lvalue()));
    std::string rhsValue = std::any_cast<std::string>(visit(ctx->rhs()));
    auto* bloc = currentCFG->getCurrentBasicBloc();
    
    // Vérifier si c'est un déréférencement : la lvalue commence par "!deref_"
    if (lvalueResult.rfind("!deref_", 0) == 0) {
        // C'est une assignation via pointeur: *p = value
        std::string pointerVar = lvalueResult.substr(7);
        bloc->addInstruction(new IRInstrStoreDeref(bloc, pointerVar, rhsValue));
    } else {
        // Assignation normale: var = value
        bloc->addInstruction(new IRInstrCopy(bloc, lvalueResult, rhsValue));
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitLvalue_id(ifccParser::Lvalue_idContext* ctx) {
    return ctx->ID()->getText();
}

antlrcpp::Any IRVisitor::visitLvalue_deref(ifccParser::Lvalue_derefContext* ctx) {
    std::string innerLvalue = std::any_cast<std::string>(visit(ctx->lvalue()));
    // On retourne avec un marqueur pour indiquer que c'est un déréférencement
    // Le marqueur "!deref_" sera détecté dans visitAssign_stmt
    return "!deref_" + innerLvalue;
}

antlrcpp::Any IRVisitor::visitLvalue_parenthese(ifccParser::Lvalue_parentheseContext* ctx) {
    return visit(ctx->lvalue());
}

antlrcpp::Any IRVisitor::visitBreak_stmt(ifccParser::Break_stmtContext* ctx) {
    auto* bloc = currentCFG->getCurrentBasicBloc();

    if (!switchBreakStack.empty()) {
        bloc->setExitTrue(switchBreakStack.back());
    } else if (!loopStack.empty()) {
        bloc->setExitTrue(loopStack.back().breakNextBlock);
    } else {
        throw std::runtime_error("`break` used outside of a loop or switch");
    }

    currentCFG->setCurrentBasicBloc(createDeadBlock(".after_break"));
    return 0;
}

antlrcpp::Any IRVisitor::visitContinue_stmt(ifccParser::Continue_stmtContext* ctx) {

    if (loopStack.empty()) {
        throw std::runtime_error("`continue` used outside of a loop");
    }

    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->setExitTrue(loopStack.back().continueNextBlock);

    currentCFG->setCurrentBasicBloc(createDeadBlock(".after_continue"));
    return 0;
}

// ─── Visiteurs d'expressions ───────────────────────────────────────
// Convention : chaque visiteur d'expression retourne un std::string
// contenant le nom du temporaire (ou de la variable) où le résultat
// est stocké.

antlrcpp::Any IRVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    std::string tmp = currentCFG->newTemp();
    int val = std::stoi(ctx->CONST()->getText());
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrConst(bloc, tmp, val));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_char(ifccParser::Expr_charContext* ctx) {
    std::string text = ctx->CHARCONST()->getText();
    int val = 0;
    if (text[1] == '\\') {
        switch (text[2]) {
            case 'n':
                val = '\n';
                break;
            case 't':
                val = '\t';
                break;
            case '\\':
                val = '\\';
                break;
            case '\'':
                val = '\'';
                break;
            default:
                val = text[2];
                break;
        }
    } else {
        val = text[1];
    }

    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrConst(bloc, tmp, val));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_id(ifccParser::Expr_idContext* ctx) {
    return ctx->ID()->getText();
}

antlrcpp::Any IRVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();

    std::string op = ctx->children[1]->getText();
    if (op == "+") {
        bloc->addInstruction(new IRInstrAdd(bloc, tmp, lhs, rhs));
    } else {
        bloc->addInstruction(new IRInstrSub(bloc, tmp, lhs, rhs));
    }
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_multdiv(ifccParser::Expr_multdivContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();

    std::string op = ctx->children[1]->getText();
    if (op == "/" || op == "%") {
        auto* divisorNode = ctx->rhs(1);
        if (auto* cctx = dynamic_cast<ifccParser::Expr_constContext*>(divisorNode)) {
            int val = std::stoi(cctx->CONST()->getText());
        }
    }

    if (op == "*") {
        bloc->addInstruction(new IRInstrMult(bloc, tmp, lhs, rhs));
    }
    else if (op == "/") {
        bloc->addInstruction(new IRInstrDiv(bloc, tmp, lhs, rhs));
    } else if (op == "%") {
        bloc->addInstruction(new IRInstrMod(bloc, tmp, lhs, rhs));
    }
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext* ctx) {
    std::string operand = std::any_cast<std::string>(visit(ctx->rhs()));
    if (ctx->children[0]->getText() == "+") {
        return operand;
    }
    else if (ctx->children[0]->getText() == "-") {
        std::string zero = currentCFG->newTemp();
        std::string tmp = currentCFG->newTemp();
        auto* bloc = currentCFG->getCurrentBasicBloc();
        bloc->addInstruction(new IRInstrConst(bloc, zero, 0));
        bloc->addInstruction(new IRInstrSub(bloc, tmp, zero, operand));
        return tmp;
    }
    else if (ctx->children[0]->getText() == "!") {
        std::string zero = currentCFG->newTemp();
        std::string tmp = currentCFG->newTemp();
        auto* bloc = currentCFG->getCurrentBasicBloc();
        bloc->addInstruction(new IRInstrConst(bloc, zero, 0));
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, operand, zero, IRInstrCmp::EQ));
        return tmp;
    }
    else {
        throw std::runtime_error("Unexpected unary operator: " + ctx->children[0]->getText());
    }
}

antlrcpp::Any IRVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext* ctx) {
    return visit(ctx->rhs());
}

antlrcpp::Any IRVisitor::visitExpr_addrof(ifccParser::Expr_addrofContext* ctx) {
    std::string lvalueVar = std::any_cast<std::string>(visit(ctx->lvalue()));
    std::string baseType = symbolTable->getType(lvalueVar);
    std::string pointerType = baseType + "*";
    std::string tmp = currentCFG->newTemp(pointerType);
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrLoadAddr(bloc, tmp, lvalueVar));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_deref(ifccParser::Expr_derefContext* ctx) {
    std::string pointerVar = std::any_cast<std::string>(visit(ctx->rhs()));
    std::string pointerType = symbolTable->getType(pointerVar);
    std::string baseType = pointerType.substr(0, pointerType.length() - 1);
    std::string tmp = currentCFG->newTemp(baseType);
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrLoadDeref(bloc, tmp, pointerVar));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_comparison(ifccParser::Expr_comparisonContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();

    std::string op = ctx->children[1]->getText();
    if (op == "<") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::LT));
    } else if (op == "<=") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::LE));
    } else if (op == ">") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::GT));
    } else if (op == ">=") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::GE));
    }
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_equality(ifccParser::Expr_equalityContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();

    std::string op = ctx->children[1]->getText();
    if (op == "==") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::EQ));
    }
    else if (op == "!=") {
        bloc->addInstruction(new IRInstrCmp(bloc, tmp, lhs, rhs, IRInstrCmp::NEQ));
    }

    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrAnd(bloc, tmp, lhs, rhs));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrOr(bloc, tmp, lhs, rhs));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrXor(bloc, tmp, lhs, rhs));
    return tmp;
}

antlrcpp::Any IRVisitor::visitWhile_stmt(ifccParser::While_stmtContext* ctx) {

    IRBasicBloc* entryBloc = currentCFG->getCurrentBasicBloc();

    IRBasicBloc* condBloc = currentCFG->addBasicBlocUnique("while_cond");
    IRBasicBloc* bodyBloc = currentCFG->addBasicBlocUnique("while_body");
    IRBasicBloc* endBloc  = currentCFG->addBasicBlocUnique("after_while");

    entryBloc->setExitTrue(condBloc);

    currentCFG->setCurrentBasicBloc(condBloc);

    std::string condTmp = std::any_cast<std::string>(visit(ctx->rhs()));

    condBloc->setTestVarName(condTmp);
    condBloc->setExitTrue(bodyBloc);
    condBloc->setExitFalse(endBloc);

    currentCFG->setCurrentBasicBloc(bodyBloc);
    loopStack.push_back({condBloc, endBloc});

    visit(ctx->block());

    loopStack.pop_back();
    if (currentCFG->getCurrentBasicBloc()->getExitTrue() == nullptr &&
        currentCFG->getCurrentBasicBloc()->getExitFalse() == nullptr) {
        currentCFG->getCurrentBasicBloc()->setExitTrue(condBloc);
    }

    currentCFG->setCurrentBasicBloc(endBloc);

    return 0;
}




antlrcpp::Any IRVisitor::visitIf_elsifelse(ifccParser::If_elsifelseContext *ctx)
{
    int ifId = ifCounter++;

    IRBasicBloc* currentTestBloc = currentCFG->getCurrentBasicBloc();

    size_t nConds = ctx->rhs().size();
    size_t nBlocks = ctx->block().size();
    bool hasElse = (nBlocks > nConds);

    std::vector<IRBasicBloc*> thenBlocs;
    for (size_t i = 0; i < nConds; i++) {
        thenBlocs.push_back(
        currentCFG->addBasicBlocUnique(".then_")
        );
    }

    std::vector<IRBasicBloc*> nextTestBlocs;
    for (size_t i = 1; i < nConds; i++) {
        nextTestBlocs.push_back(currentCFG->addBasicBlocUnique(".test_"));
    }

    IRBasicBloc* elseBloc = nullptr;
    if (hasElse) {
        elseBloc = currentCFG->addBasicBlocUnique(".else_");
    }

    IRBasicBloc* exitBloc = currentCFG->addBasicBlocUnique(".if_exit_");

    for (size_t i = 0; i < nConds; i++) {
        currentCFG->setCurrentBasicBloc(currentTestBloc);

        std::string testVarName = std::any_cast<std::string>(visit(ctx->rhs(i)));

        IRBasicBloc* falseDest = nullptr;
        if (i + 1 < nConds) {
            falseDest = nextTestBlocs[i];
        } else if (hasElse) {
            falseDest = elseBloc;
        } else {
            falseDest = exitBloc;
        }

        currentTestBloc->setTestVarName(testVarName);
        currentTestBloc->setExitTrue(thenBlocs[i]);
        currentTestBloc->setExitFalse(falseDest);

        // Bloc then / else-if
        currentCFG->setCurrentBasicBloc(thenBlocs[i]);
        visit(ctx->block(i));

        // Ne relier à exit que si le bloc courant n'a pas déjà une sortie terminale
        if (currentCFG->getCurrentBasicBloc()->getExitTrue() == nullptr &&
            currentCFG->getCurrentBasicBloc()->getExitFalse() == nullptr) {
            currentCFG->getCurrentBasicBloc()->setExitTrue(exitBloc);
        }

        if (i + 1 < nConds) {
            currentTestBloc = nextTestBlocs[i];
        }
    }

    if (hasElse) {
        currentCFG->setCurrentBasicBloc(elseBloc);
        visit(ctx->block(nBlocks - 1));

        if (currentCFG->getCurrentBasicBloc()->getExitTrue() == nullptr &&
            currentCFG->getCurrentBasicBloc()->getExitFalse() == nullptr) {
            currentCFG->getCurrentBasicBloc()->setExitTrue(exitBloc);
        }
    }

    currentCFG->setCurrentBasicBloc(exitBloc);
    return 0;
}

antlrcpp::Any IRVisitor::visitSwitch_stmt(ifccParser::Switch_stmtContext* ctx) {

    IRBasicBloc* entryBloc = currentCFG->getCurrentBasicBloc();

    // Evaluation de l'expression contenue dans le switch
    std::string switchTmp = std::any_cast<std::string>(visit(ctx->rhs()));

    // Bloc de sortie du switch
    IRBasicBloc* endSwitch = currentCFG->addBasicBlocUnique("after_switch");

    // Blocs pour faire les comparaisons
    std::vector<IRBasicBloc*> testBlocs;

    // Blocs pour les codes à exécuter pour chaque case
    std::vector<IRBasicBloc*> caseBlocs;

    for (size_t i = 0; i < ctx->switch_case().size(); ++i) {
        testBlocs.push_back(currentCFG->addBasicBlocUnique("switch_test"));
        caseBlocs.push_back(currentCFG->addBasicBlocUnique("switch_case"));
    }

    // Bloc default si il existe
    IRBasicBloc* defaultBloc = nullptr;
    if (ctx->switch_default() != nullptr) {
        defaultBloc = currentCFG->addBasicBlocUnique("switch_default");
    }

    if (!testBlocs.empty()) {
        entryBloc->setExitTrue(testBlocs.front());
    } else if (defaultBloc != nullptr) {
        entryBloc->setExitTrue(defaultBloc);
    } else {
        entryBloc->setExitTrue(endSwitch);
    }

    switchBreakStack.push_back(endSwitch);

    // Chaque bloc de test vérifie si la valeur du switch correspond à un case
    for (size_t i = 0; i < testBlocs.size(); ++i) {
        currentCFG->setCurrentBasicBloc(testBlocs[i]);
        auto* switchCase = ctx->switch_case(i);
        std::string caseValueTmp = std::any_cast<std::string>(visit(switchCase->switch_value()));

        // cmpTmp vaut 1 si switchTmp == caseValueTmp, sinon 0
        std::string cmpTmp = currentCFG->newTemp();
        testBlocs[i]->addInstruction(new IRInstrCmp(testBlocs[i], cmpTmp, switchTmp, caseValueTmp, IRInstrCmp::EQ));
        testBlocs[i]->setTestVarName(cmpTmp);

        // Si la comparaison est vraie, on exécute le code du case
        testBlocs[i]->setExitTrue(caseBlocs[i]);

        IRBasicBloc* falseDest = nullptr;

        // Est-ce qu'on a encore un bloc de test après le bloc actuel
        if (i + 1 < testBlocs.size()) {
            // Si le test du case courant échoue et qu’il y a un autre case après on teste le suivant.
            falseDest = testBlocs[i + 1];
        } else if (defaultBloc != nullptr) {
            // Si aucun case n'a matché jusque-là on va sur default s'il existe
            falseDest = defaultBloc;
        } else {
            // Sinon c'est qu'aucune case ne correspond et on sort du switch
            falseDest = endSwitch;
        }
        testBlocs[i]->setExitFalse(falseDest);
    }

    // Chaque bloc case contient les instructions du case correspondant
    for (size_t i = 0; i < caseBlocs.size(); ++i) {
        currentCFG->setCurrentBasicBloc(caseBlocs[i]);

        for (auto* stmt : ctx->switch_case(i)->stmt()) {
            visit(stmt);
        }

        // Le dernier bloc du case peut être différent si des sous-blocs ont été créés
        IRBasicBloc* tailBloc = currentCFG->getCurrentBasicBloc();
        if (tailBloc->getExitTrue() == nullptr && tailBloc->getExitFalse() == nullptr) {
            if (i + 1 < caseBlocs.size()) {
                // Si on n'a pas de break ni de return, on continue dans le case suivant
                tailBloc->setExitTrue(caseBlocs[i + 1]);
            } else if (defaultBloc != nullptr) {
                // Une fois qu'on atteint le dernier case on continue dans default s'il existe
                tailBloc->setExitTrue(defaultBloc);
            } else {
                // Sinon c'est qu'on a fini le switch
                tailBloc->setExitTrue(endSwitch);
            }
        }
    }

    if (defaultBloc != nullptr) {
        // Le bloc default est atteint si aucun case ne correspond ou si le dernier case tombe dedans sans break
        currentCFG->setCurrentBasicBloc(defaultBloc);
        for (auto* stmt : ctx->switch_default()->stmt()) {
            visit(stmt);
        }

        IRBasicBloc* tailBloc = currentCFG->getCurrentBasicBloc();
        if (tailBloc->getExitTrue() == nullptr && tailBloc->getExitFalse() == nullptr) {
            tailBloc->setExitTrue(endSwitch); // Default mène à la fin du switch.
        }
    }

    switchBreakStack.pop_back();
    currentCFG->setCurrentBasicBloc(endSwitch);

    return 0;
}

antlrcpp::Any IRVisitor::visitSwitch_value(ifccParser::Switch_valueContext* ctx) {
    int val = 0;

    if (ctx->CONST() != nullptr) {
        val = std::stoi(ctx->CONST()->getText());
    } else {
        val = parseCharLiteralValue(ctx->CHARCONST()->getText());
    }

    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrConst(bloc, tmp, val));
    return tmp;
}


antlrcpp::Any IRVisitor::visitExpr_getchar(ifccParser::Expr_getcharContext* ctx) {
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrGetchar(bloc, tmp));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_putchar(ifccParser::Expr_putcharContext* ctx) {
    std::string arg;
    auto* bloc = currentCFG->getCurrentBasicBloc();
    auto* ioArg = ctx->io_arg();
    if (ioArg->CONST()) {
        arg = currentCFG->newTemp();
        bloc->addInstruction(new IRInstrConst(bloc, arg, std::stoi(ioArg->CONST()->getText())));
    } else if (ioArg->CHARCONST()) {
        std::string text = ioArg->CHARCONST()->getText();
        int val = (text[1] == '\\') ? text[2] : text[1];
        arg = currentCFG->newTemp();
        bloc->addInstruction(new IRInstrConst(bloc, arg, val));
    } else if (ioArg->ID()) {
        arg = ioArg->ID()->getText();
    } else {
        arg = currentCFG->newTemp();
        bloc->addInstruction(new IRInstrGetchar(bloc, arg));
    }

    std::string tmp = currentCFG->newTemp();
    bloc->addInstruction(new IRInstrPutchar(bloc, tmp, arg));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_funcCall(ifccParser::Expr_funcCallContext* ctx) {
    std::string funcName = ctx->ID()->getText();
    std::vector<std::string> args;
    if (ctx->rhsList()) {
        auto rhsListResult = visit(ctx->rhsList());
        args = std::any_cast<std::vector<std::string>>(rhsListResult);
    }

    std::string dest = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrCall(bloc, dest, funcName, args));

    return dest;
}

antlrcpp::Any IRVisitor::visitRhsList(ifccParser::RhsListContext* ctx) {
    std::vector<std::string> args;
    for (auto* rhs : ctx->rhs()) {
        std::string arg = std::any_cast<std::string>(visit(rhs));
        args.push_back(arg);
    }
    return args;
}
