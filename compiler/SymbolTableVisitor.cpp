#include "SymbolTableVisitor.h"
#include <cstdlib>
#include <iostream>

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx) {
    visitChildren(ctx);
    symbolTable.checkUnusedVariables();
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) {
    return visitChildren(ctx);
}


antlrcpp::Any SymbolTableVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);

    visit(ctx->rhs());   

    return 0;
}


antlrcpp::Any SymbolTableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    visit(ctx->rhs());
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx) {
    std::string varName = ctx->ID()->getText();

    symbolTable.addSymbol(varName);  // Erreur automatique si déjà déclaré

    if (ctx->rhs() != nullptr) {
        visit(ctx->rhs());   
        symbolTable.getSymbol(varName).used = true;
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_id(ifccParser::Expr_idContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_const(ifccParser::Expr_constContext *ctx) {
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_char(ifccParser::Expr_charContext *ctx) {
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) {
    return visit(ctx->rhs());
}

antlrcpp::Any SymbolTableVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) {
    visit(ctx->rhs());
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_comparison(ifccParser::Expr_comparisonContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_equality(ifccParser::Expr_equalityContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_and(ifccParser::Expr_andContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_xor(ifccParser::Expr_xorContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_or(ifccParser::Expr_orContext *ctx) {
    visit(ctx->rhs(0));
    visit(ctx->rhs(1));
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_getchar(ifccParser::Expr_getcharContext *ctx) {
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_putchar(ifccParser::Expr_putcharContext *ctx) {
    auto* ioArg = ctx->io_arg();
    if (ioArg->ID()) {
        checkVariableUsed(ioArg->ID()->getText());
    }
    return 0;
}

void SymbolTableVisitor::checkVariableUsed(const std::string& varName) {
    // getSymbol() sort en erreur si la variable n'existe pas
    symbolTable.getSymbol(varName).used = true;
}
