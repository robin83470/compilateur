#pragma once

#include <unordered_map>

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTable.h"

class SymbolTableVisitor : public ifccBaseVisitor {
    public:
        SymbolTable symbolTable;

        const std::string& getStorageName(const ifccParser::DeclaratorContext* ctx) const;
        const std::string& getStorageName(const ifccParser::Assign_stmtContext* ctx) const;
        const std::string& getStorageName(const ifccParser::Expr_idContext* ctx) const;

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDeclarator(ifccParser::DeclaratorContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) override;
        virtual antlrcpp::Any visitExpr_id(ifccParser::Expr_idContext *ctx) override;
        virtual antlrcpp::Any visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx) override;
        virtual antlrcpp::Any visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx) override;
        virtual antlrcpp::Any visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) override;
        virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext *ctx) override;
        virtual antlrcpp::Any visitExpr_char(ifccParser::Expr_charContext *ctx) override;
        virtual antlrcpp::Any visitExpr_comparison(ifccParser::Expr_comparisonContext *ctx) override;
        virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext *ctx) override;
        virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext *ctx) override;
        virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext *ctx) override;
        virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext *ctx) override;
        virtual antlrcpp::Any visitExpr_getchar(ifccParser::Expr_getcharContext *ctx) override;
        virtual antlrcpp::Any visitExpr_putchar(ifccParser::Expr_putcharContext *ctx) override;
        virtual antlrcpp::Any visitBlock(ifccParser::BlockContext *ctx) override;
    private:
        void checkVariableUsed(const std::string& varName);



        std::unordered_map<const ifccParser::DeclaratorContext*, std::string> declaratorStorageNames;
        std::unordered_map<const ifccParser::Assign_stmtContext*, std::string> assignStorageNames;
        std::unordered_map<const ifccParser::Expr_idContext*, std::string> exprStorageNames;
};
