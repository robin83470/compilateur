#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTableVisitor.h"

class CodeGenVisitor : public ifccBaseVisitor
{
private:
    std::map<std::string, SymbolTableVisitor::SymbolInfo> symbolTable;

public:
    CodeGenVisitor(std::map<std::string, SymbolTableVisitor::SymbolInfo> symbols);

    virtual std::any visitProg(ifccParser::ProgContext *ctx) override;
    virtual std::any visitDeclarator(ifccParser::DeclaratorContext *ctx) override;
    virtual std::any visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) override;
    virtual std::any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
    virtual std::any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;

    virtual std::any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) override;
    virtual std::any visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) override;
    virtual std::any visitExpr_id(ifccParser::Expr_idContext *ctx) override;
    virtual std::any visitExpr_const(ifccParser::Expr_constContext *ctx) override;

    virtual std::any visitRhs(ifccParser::RhsContext *ctx) override {
        return visitChildren(ctx);
    }
};