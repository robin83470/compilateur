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
        virtual antlrcpp::Any visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx) override;
        virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext *ctx) override;
        virtual antlrcpp::Any visitExpr_id(ifccParser::Expr_idContext *ctx) override;
        virtual antlrcpp::Any visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx) override;
        virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) override;
        virtual antlrcpp::Any visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) override;
        virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext *ctx) override;
        virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext *ctx) override;
        virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext *ctx) override;


        // virtual antlrcpp::Any visitRhs(ifccParser::RhsContext *ctx) override
        // {
        //         return visitChildren(ctx);
        // }

};
