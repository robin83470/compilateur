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
    visit(ctx->rhs());
    return 0;
    // ifccParser::RhsContext *rhsCtx = ctx->rhs();

    // if (rhsCtx->CONST()) {
    //     int retval = stoi(rhsCtx->CONST()->getText());
    //     std::cout << "    movl $" << retval << ", %eax\n";
    // } 
    // else if (rhsCtx->ID()) {
    //     std::string varName = rhsCtx->ID()->getText();
    //     int offset = symbolTable[varName].index; 
    //     std::cout << "    movl " << offset << "(%rbp), %eax\n"; 
    // }

    // return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx)
{
    for (auto decl : ctx->declarator()) {
        visit(decl);
    }
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx)
{
    std::string varName = ctx->ID()->getText();
    int offset = symbolTable[varName].index;

    if (ctx->EQUAL()) {
        visit(ctx->rhs());   
        std::cout << "    movl %eax, " << offset << "(%rbp)\n";
    } else {
        std::cout << "    movl $0, " << offset << "(%rbp)\n";
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx)
{
    std::string varName = ctx->ID()->getText();
    int offset = symbolTable[varName].index;

    visit(ctx->rhs());   
    std::cout << "    movl %eax, " << offset << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_const(ifccParser::Expr_constContext *ctx)
{
    int val = std::stoi(ctx->CONST()->getText());
    std::cout << "    movl $" << val << ", %eax\n";
    return 0;
}



antlrcpp::Any CodeGenVisitor::visitExpr_id(ifccParser::Expr_idContext *ctx)
{
    std::string name = ctx->ID()->getText();
    int offset = symbolTable[name].index;
    std::cout << "    movl " << offset << "(%rbp), %eax\n";
    return 0;
}

antlrcpp::Any CodeGenVisitor:: visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx)
{
    // gauche
    visit(ctx->expr(0));

    std::cout << "    pushl %eax\n";

    // droite
    visit(ctx->expr(1));

    std::cout << "    popl %ebx\n";

    std::string op = ctx->children[1]->getText();

    if(op == "*") {
        std::cout << "    imull %ebx, %eax\n";
    }
    else if(op == "/") {
        std::cout << "    movl %eax, %ecx\n";
        std::cout << "    movl %ebx, %eax\n";
        std::cout << "    cdq\n";
        std::cout << "    idivl %ecx\n";
    }
  

    return 0;
}