#include "IRVisitor.h"
#include "IR.h"

IRVisitor::IRVisitor(SymbolTable* symbolTable)
    : symbolTable(symbolTable) {}

IRControlFlowGraph* IRVisitor::buildIr(antlr4::tree::ParseTree* tree) {
    currentCFG = new IRControlFlowGraph(symbolTable);
    currentCFG->addBasicBloc(".entry");

    // Réserver le temporaire pour la valeur de retour
    symbolTable->addSymbol("!retval");

    visit(tree);

    return currentCFG;
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext* ctx) {
    for (auto* stmt : ctx->stmt()) {
        visit(stmt);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) {
    std::string tmp = std::any_cast<std::string>(visit(ctx->rhs()));
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrCopy(bloc, "!retval", tmp));
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
    if (ctx->EQUAL()) {
        std::string tmp = std::any_cast<std::string>(visit(ctx->rhs()));
        auto* bloc = currentCFG->getCurrentBasicBloc();
        bloc->addInstruction(new IRInstrCopy(bloc, varName, tmp));
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) {
    std::string varName = ctx->ID()->getText();
    std::string tmp = std::any_cast<std::string>(visit(ctx->rhs()));
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrCopy(bloc, varName, tmp));
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
            if (val == 0) {
                std::cerr << "warning: division by zero " << std::endl;
            }
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

    std::string zero = currentCFG->newTemp();
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();
    bloc->addInstruction(new IRInstrConst(bloc, zero, 0));
    bloc->addInstruction(new IRInstrSub(bloc, tmp, zero, operand));
    return tmp;
}

antlrcpp::Any IRVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext* ctx) {
    return visit(ctx->rhs());
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
    else if (op == "==") {
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





antlrcpp::Any IRVisitor::visitIf_elsifelse(ifccParser::If_elsifelseContext *ctx)
{
    IRBasicBloc* currentTestBloc = currentCFG->getCurrentBasicBloc();

    size_t nConds = ctx->rhs().size();
    size_t nBlocks = ctx->block().size();

    // On prépare les blocs "then"
    std::vector<IRBasicBloc*> thenBlocs;
    for (size_t i = 0; i < nConds; i++) {
        thenBlocs.push_back(currentCFG->addBasicBloc(".then_" + std::to_string(i)));
    }

    // On prépare les blocs de test suivants / else
    std::vector<IRBasicBloc*> falseBlocs;
    for (size_t i = 1; i < nConds; i++) {
        falseBlocs.push_back(currentCFG->addBasicBloc(".test_" + std::to_string(i)));
    }

    IRBasicBloc* elseBloc = nullptr;
    if (nBlocks > nConds) {
        elseBloc = currentCFG->addBasicBloc(".else");
    }


    IRBasicBloc* exitBloc = currentCFG->addBasicBloc(".if_exit");

    for (size_t i = 0; i < nConds; i++) {
        currentCFG->setCurrentBasicBloc(currentTestBloc);

        std::string testVarName = std::any_cast<std::string>(visit(ctx->rhs(i)));

        IRBasicBloc* falseDest = nullptr;
        if (i + 1 < nConds) {
            falseDest = falseBlocs[i];
        } else if (elseBloc != nullptr) {
            falseDest = elseBloc;
        } else {
            falseDest = exitBloc;
        }

        currentTestBloc->setTestVarName(testVarName);
        currentTestBloc->setExitTrue(thenBlocs[i]);
        currentTestBloc->setExitFalse(falseDest);

        // Corps du then / elsif
        currentCFG->setCurrentBasicBloc(thenBlocs[i]);
        visit(ctx->block(i));
        currentCFG->getCurrentBasicBloc()->setExitTrue(exitBloc);

        if (i + 1 < nConds) {
            currentTestBloc = falseBlocs[i];
        }
    }

    // else final éventuel
    if (elseBloc != nullptr) {
        currentCFG->setCurrentBasicBloc(elseBloc);
        visit(ctx->block(nBlocks - 1));
        currentCFG->getCurrentBasicBloc()->setExitTrue(exitBloc);
    }

    currentCFG->setCurrentBasicBloc(exitBloc);
    return 0;
}