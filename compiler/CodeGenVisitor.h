#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"


class  CodeGenVisitor : public ifccBaseVisitor {
	public:
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
        virtual antlrcpp::Any visitReturn_const_stmt(ifccParser::Return_const_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturn_var_stmt(ifccParser::Return_var_stmtContext *ctx) override;
};

