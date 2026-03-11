#include "IRVisitor.h"
#include "IR.h"

IRVisitor::IRVisitor(SymbolTable* symbolTable)
    : symbolTable(symbolTable) {}

IRControlFlowGraph* IRVisitor::buildIr(antlr4::tree::ParseTree* tree) {
    // Créer le CFG, le visiter, le retourner
    currentCFG = new IRControlFlowGraph(symbolTable);
    currentCFG->addBasicBloc(".entry");

    visit(tree);

    return currentCFG;
}

antlrcpp::Any IRVisitor::visitProg(ifccParser::ProgContext* ctx) {
    // TODO: implémenter la logique
    // Visiter tous les statements
    for (auto* stmt : ctx->stmt()) {
        visit(stmt);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitReturn_stmt(ifccParser::Return_stmtContext* ctx) {
    // TODO: implémenter la logique
    // 1. Visiter l'expression rhs → récupérer le nom du temporaire résultat
    // 2. Générer un IRInstrCopy vers un registre spécial (ex: "!retval") ou
    //    directement marquer ce temporaire comme valeur de retour
    return 0;
}

antlrcpp::Any IRVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) {
    for (auto* decl : ctx->declarator()) {
        visit(decl);
    }
    return 0;
}

antlrcpp::Any IRVisitor::visitDeclarator(ifccParser::DeclaratorContext* ctx) {
    // TODO: implémenter la logique
    // Si initialisé (EQUAL rhs) :
    //   1. Visiter rhs → nom du temporaire
    //   2. Générer IRInstrCopy de tmp vers la variable
    return 0;
}

antlrcpp::Any IRVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) {
    // TODO: implémenter la logique
    // 1. Visiter rhs → nom du temporaire
    // 2. Générer IRInstrCopy de tmp vers la variable
    return 0;
}

// ─── Visiteurs d'expressions ───────────────────────────────────────
// Convention : chaque visiteur d'expression retourne un std::string
// contenant le nom du temporaire (ou de la variable) où le résultat
// est stocké.

antlrcpp::Any IRVisitor::visitExpr_const(ifccParser::Expr_constContext* ctx) {
    // TODO: implémenter la logique
    // 1. Créer un temporaire via currentCFG->newTemp()
    // 2. Ajouter IRInstrConst(bloc, tmp, valeur)
    // 3. Retourner le nom du temporaire
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_id(ifccParser::Expr_idContext* ctx) {
    // TODO: implémenter la logique
    // Retourner directement le nom de la variable (elle est déjà en mémoire)
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext* ctx) {
    // TODO: implémenter la logique
    // 1. Visiter lhs et rhs → deux noms de temporaires
    // 2. Créer un temporaire résultat
    // 3. Ajouter IRInstrAdd ou IRInstrSub selon l'opérateur
    // 4. Retourner le nom du temporaire résultat
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_multdiv(ifccParser::Expr_multdivContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext* ctx) {
    return visit(ctx->rhs());
}

antlrcpp::Any IRVisitor::visitExpr_comparison(ifccParser::Expr_comparisonContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_equality(ifccParser::Expr_equalityContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_and(ifccParser::Expr_andContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_or(ifccParser::Expr_orContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}

antlrcpp::Any IRVisitor::visitExpr_xor(ifccParser::Expr_xorContext* ctx) {
    // TODO: implémenter la logique
    return std::string("");
}
