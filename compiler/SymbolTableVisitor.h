#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"
#include "SymbolTable.h"

class SymbolTableVisitor : public ifccBaseVisitor {
    public:
        SymbolTable symbolTable;

        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override;
        virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) override;
        virtual antlrcpp::Any visitDeclarator(ifccParser::DeclaratorContext *ctx) override;
        virtual antlrcpp::Any visitPointer_prefix(ifccParser::Pointer_prefixContext *ctx) override;
        virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext *ctx) override;
        virtual antlrcpp::Any visitLvalue_id(ifccParser::Lvalue_idContext *ctx) override;
        virtual antlrcpp::Any visitLvalue_deref(ifccParser::Lvalue_derefContext *ctx) override;
        virtual antlrcpp::Any visitLvalue_parenthese(ifccParser::Lvalue_parentheseContext *ctx) override;
        virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) override;
        virtual antlrcpp::Any visitExpr_id(ifccParser::Expr_idContext *ctx) override;
        virtual antlrcpp::Any visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx) override;
        virtual antlrcpp::Any visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx) override;
        virtual antlrcpp::Any visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) override;
        virtual antlrcpp::Any visitExpr_addrof(ifccParser::Expr_addrofContext *ctx) override;
        virtual antlrcpp::Any visitExpr_deref(ifccParser::Expr_derefContext *ctx) override;
        virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext *ctx) override;
        virtual antlrcpp::Any visitExpr_negconst(ifccParser::Expr_negconstContext *ctx) override;
        virtual antlrcpp::Any visitExpr_char(ifccParser::Expr_charContext *ctx) override;
        virtual antlrcpp::Any visitExpr_comparison(ifccParser::Expr_comparisonContext *ctx) override;
        virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext *ctx) override;
        virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext *ctx) override;
        virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext *ctx) override;
        virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext *ctx) override;
        virtual antlrcpp::Any visitExpr_getchar(ifccParser::Expr_getcharContext *ctx) override;
        virtual antlrcpp::Any visitExpr_putchar(ifccParser::Expr_putcharContext *ctx) override;
        virtual antlrcpp::Any visitPutchar_stmt(ifccParser::Putchar_stmtContext *ctx) override;
        virtual antlrcpp::Any visitExpr_funcCall(ifccParser::Expr_funcCallContext* ctx) override;
        virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext* ctx) override;
        virtual antlrcpp::Any visitSwitch_stmt(ifccParser::Switch_stmtContext *ctx) override;
        virtual antlrcpp::Any visitSwitch_value(ifccParser::Switch_valueContext *ctx) override;

    private:
        void checkVariableUsed(const std::string& varName);
        void requireType(const std::string& actual, const std::string& expected, const std::string& where);
        std::string addPointerLevel(const std::string& type) const;
        std::string removePointerLevel(const std::string& type, const std::string& where) const;
};
