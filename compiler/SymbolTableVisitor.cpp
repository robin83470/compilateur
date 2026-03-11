#include "SymbolTableVisitor.h"
#include <iostream>

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx) {
    visitChildren(ctx);
    checkUnusedVariables();
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) {
    return visitChildren(ctx);
}


antlrcpp::Any SymbolTableVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);
    symbolTable[varName].used = true;

    visit(ctx->rhs());   

    return 0;
}


antlrcpp::Any SymbolTableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    visit(ctx->rhs());
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx) {
    std::string varName = ctx->ID()->getText();

    if (symbolTable.count(varName) > 0) {
        std::cerr << "Erreur : la variable '" << varName << "' est déjà déclarée." << std::endl;
        exit(1);
    }

    currentOffset -= 4;
    SymbolInfo info;
    info.index = currentOffset;
    info.declared = true;
    info.used = false;

    symbolTable[varName] = info;

    if (ctx->rhs() != nullptr) {
        visit(ctx->rhs());   
        symbolTable[varName].used = true;
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


void SymbolTableVisitor::checkVariableUsed(const std::string& varName) {
    if (symbolTable.count(varName) == 0) {
        std::cerr << "Erreur : la variable '" << varName << "' a été utilisée mais n'a pas été déclarée." << std::endl;
        exit(1);
    }
    
    symbolTable[varName].used = true;
}

void SymbolTableVisitor::checkUnusedVariables() {
    for (const auto& pair : symbolTable) {
        const std::string& varName = pair.first;
        const SymbolInfo& info = pair.second;
        
        if (!info.used) {
            std::cerr << "Avertissement : la variable '" << varName << "' a été déclarée mais n'est pas utilisée." << std::endl;
        }
    }
}
