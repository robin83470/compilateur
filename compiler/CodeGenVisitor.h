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

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitDeclarator(ifccParser::DeclaratorContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::UnaryExprContext *ctx) override;
        virtual antlrcpp::Any visitExpr_parenthese(ifccParser::UnaryExprContext *ctx) override;
        virtual antlrcpp::Any visitRhs(ifccParser::RhsContext *ctx) override
        {
                return visitChildren(ctx);
        }
};
