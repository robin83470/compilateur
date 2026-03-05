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
        
    int n = ctx->expr().size();
    //std::cout << "# "<< n <<" children of type expr\n" ;
    for (int i = 0; i < n; i++) {
        visit(ctx->expr(i)); 
    }
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
   std::string name = ctx->VAR()->getText();

   int offset = mem[name].index;

    std::cout << "    movl -" << offset << "(%rbp), %eax\n";

    return 0;
}




antlrcpp::Any CodeGenVisitor::visitAssignment_decla_const(
    ifccParser::Assignment_decla_constContext *ctx)
{
    std::string name = ctx->v1->getText();
    int c = stoi(ctx->c1->getText());

    int offset = mem[name].index;

    std::cout << "    movl $" << c << ", -" << offset << "(%rbp)\n";
    
    return 0;
}


antlrcpp::Any CodeGenVisitor::visitAssignment_decla_var(ifccParser::Assignment_decla_varContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    std::string v2 = ctx->v2->getText();


    int off1 = mem[v1].index;
    int off2 = mem[v2].index;


    std::cout << "    movl -" << off2 << "(%rbp), %eax\n";
    std::cout << "    movl %eax, -" << off1 << "(%rbp)\n";
    
    return 0;
    
}



antlrcpp::Any CodeGenVisitor::visitAssignment_vv(ifccParser::Assignment_vvContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    std::string v2 = ctx->v2->getText();

    int off1 = mem[v1].index;
    int off2 = mem[v2].index;

    std::cout << "    movl -" << off2 << "(%rbp), %eax\n";
    std::cout << "    movl %eax, -" << off1 << "(%rbp)\n";

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitAssignment_vc(ifccParser::Assignment_vcContext *ctx)
{
    std::string v1 = ctx->v1->getText();
    int c = stoi(ctx->c1->getText());

    int offset = mem[v1].index;
    std::cout << "    movl $" << c << ", -" << offset << "(%rbp)\n";
    
    return 0;
}


antlrcpp::Any CodeGenVisitor::visitAssignment_decla(ifccParser::Assignment_declaContext *ctx)
{
    std::string name = ctx->v1->getText();
    int c = 0;

    int offset = mem[name].index;

    std::cout << "    movl $" << c << ", -" << offset << "(%rbp)\n";
    
    return 0;
}
