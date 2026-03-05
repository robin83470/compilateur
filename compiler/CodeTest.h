#pragma once


#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

struct Symbol{
        int index;
        bool used;
        int decline;
};

class  CodeTest : public ifccBaseVisitor {
	public:
        std::unordered_map<std::string,Symbol> mem; // en octets
        std::vector<std::string> erreur_;
        int nextOffset = 0;
        CodeTest() = default;
        ~CodeTest() = default;
        virtual antlrcpp::Any visitProg(ifccParser::ProgContext *ctx) override ;
        virtual antlrcpp::Any visitReturn_const_stmt(ifccParser::Return_const_stmtContext *ctx) override;
        virtual antlrcpp::Any visitReturn_var_stmt(ifccParser::Return_var_stmtContext *ctx) override;
        virtual antlrcpp::Any visitAssignment_decla_const(ifccParser::Assignment_decla_constContext *ctx) override;
        virtual antlrcpp::Any visitAssignment_decla_var(ifccParser::Assignment_decla_varContext *ctx) override;
        virtual antlrcpp::Any visitAssignment_vv(ifccParser::Assignment_vvContext *ctx) override;
        virtual antlrcpp::Any visitAssignment_vc(ifccParser::Assignment_vcContext *ctx) override;
        virtual antlrcpp::Any visitAssignment_decla(ifccParser::Assignment_declaContext *ctx) override;
        
};

