#include "CodeGenVisitor.h"

CodeGenVisitor::CodeGenVisitor(std::map<std::string, SymbolTableVisitor::SymbolInfo> symbols) {
    this->symbolTable = symbols;
}

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    #ifdef __APPLE__
    std::cout<< ".globl _main\n" ;
    std::cout<< " _main: \n" ;
    #else
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;
    #endif

    // Prologue
    std::cout<< "    pushq %rbp\n" ;
    std::cout<< "    movq %rsp, %rbp\n";

    // Body
    for (auto stmt : ctx->stmt()) {
        this->visit(stmt);
    }
    
    // Epilogue
    std::cout<<  "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    ifccParser::RhsContext *rhsCtx = ctx->rhs();

    if (rhsCtx->CONST()) {
        int retval = stoi(rhsCtx->CONST()->getText());
        std::cout << "    movl $" << retval << ", %eax\n";
    } 
    else if (rhsCtx->ID()) {
        std::string varName = rhsCtx->ID()->getText();
        int offset = symbolTable[varName].index; 
        std::cout << "    movl " << offset << "(%rbp), %eax\n"; 
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx)
{
    for (auto decl : ctx->declarator()) {
        this->visit(decl);
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx)
{
    if (ctx->EQUAL()) { 
        std::string varName = ctx->ID()->getText();
        int offset = symbolTable[varName].index;

        ifccParser::RhsContext *rhsCtx = ctx->rhs();

        if (rhsCtx->CONST()) {
            int val = stoi(rhsCtx->CONST()->getText());
            std::cout << "    movl $" << val << ", " << offset << "(%rbp)\n";
        } 
        else if (rhsCtx->ID()) {
            std::string rhsName = rhsCtx->ID()->getText();
            int rhsOffset = symbolTable[rhsName].index;
            std::cout << "    movl " << rhsOffset << "(%rbp), %eax\n";
            std::cout << "    movl %eax, " << offset << "(%rbp)\n";
        }
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    std::string varName = ctx->ID()->getText();
    int offset = symbolTable[varName].index;

    ifccParser::RhsContext *rhsCtx = ctx->rhs();

    if (rhsCtx->CONST()) {
        int val = stoi(rhsCtx->CONST()->getText());
        std::cout << "    movl $" << val << ", " << offset << "(%rbp)\n";
    } 
    else if (rhsCtx->ID()) {
        std::string rhsName = rhsCtx->ID()->getText();
        int rhsOffset = symbolTable[rhsName].index;
        std::cout << "    movl " << rhsOffset << "(%rbp), %eax\n";
        std::cout << "    movl %eax, " << offset << "(%rbp)\n";
    }

    return 0;
}

