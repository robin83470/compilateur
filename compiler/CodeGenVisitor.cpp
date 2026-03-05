#include "CodeGenVisitor.h"

antlrcpp::Any CodeGenVisitor::visitProg(ifccParser::ProgContext *ctx) 
{
    #ifdef __APPLE__
    std::cout<< ".globl _main\n" ;
    std::cout<< " _main: \n" ;
    #else
    std::cout<< ".globl main\n" ;
    std::cout<< " main: \n" ;
    #endif

    std::cout << "# prologue\n";
    std::cout << "pushq %rbp # save %rbp on the stack\n";
    std::cout << "movq %rsp, %rbp # define %rbp for the current function\n";


    this->visit( ctx->return_stmt() );
    

    std::cout << "# epilogue\n";
    std::cout << "popq %rbp # restore %rbp from the stack\n";
    std::cout << "    ret\n";

    return 0;
}




antlrcpp::Any CodeGenVisitor::visitReturn_const_stmt(ifccParser::Return_const_stmtContext *ctx)
{

    int retval = stoi(ctx->CONST()->getText());
    
    std::cout << "    movl $"<<retval<<", %eax\n" ;

    return 0;

}



antlrcpp::Any CodeGenVisitor::visitReturn_var_stmt(ifccParser::Return_var_stmtContext *ctx)
{   
   

    return 0;
}