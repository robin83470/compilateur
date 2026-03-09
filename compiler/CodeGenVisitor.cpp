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
    int localBytes = 0;
    for (const auto &entry : symbolTable) {
        int bytes = -entry.second.index;
        if (bytes > localBytes) {
            localBytes = bytes;
        }
    }
    int stackSize = ((localBytes + 15) / 16) * 16;
    if (stackSize > 0) {
        std::cout << "    subq $" << stackSize << ", %rsp\n";
    }

    // Body
    for (auto stmt : ctx->stmt()) {
        this->visit(stmt);
    }

    // Epilogue
    std::cout<< "    movq %rbp, %rsp\n";
    std::cout<< "    popq %rbp\n";
    std::cout << "    ret\n";

    return 0;
}


antlrcpp::Any CodeGenVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx)
{
    // Evaluate any RHS expression; convention: result in %eax.
    visit(ctx->rhs());
    return 0;
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
    }
    else {
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

antlrcpp::Any CodeGenVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx){
        visit(ctx->rhs(0)); // gauche dans %eax
        std::cout << "    pushq %rax\n";
        visit(ctx->rhs(1)); // droite dans %eax
        std::cout << "    popq %rcx\n"; // gauche dans %ebx
        std::string op = ctx->children[1]->getText();
        if (op == "+") {
            std::cout << "    addl %ecx, %eax\n"; // %eax = droite + gauche
        } else {
            std::cout << "    subl %eax, %ecx\n"; // %ecx = gauche - droite
            std::cout << "    movl %ecx, %eax\n"; // résultat dans %eax
        }
        return 0;

    }
antlrcpp::Any CodeGenVisitor::visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx)
{
    // Visite du côté gauche
    visit(ctx->rhs(0));
    // %eax contient maintenant le résultat du lhs
    // On sauvegarde lhs dans %eax

    // Visite du côté droit
    visit(ctx->rhs(1));
    // %eax contient maintenant le résultat du rhs
    // On doit charger lhs dans %ebx pour imul/idiv
    std::cout << "    movl %eax, %ebx\n"; // déplacer rhs dans %ebx
    std::cout << "    movl %eax, %eax\n"; // garder rhs en %eax pour idiv si besoin

    std::string op = ctx->children[1]->getText();

    if(op == "*") {
        // imul lhs, rhs → résultat dans %eax
        std::cout << "    imull %ebx, %eax\n";
    }
    else if(op == "/") {
        // rhs div lhs
        std::cout << "    movl %ebx, %eax\n"; // mettre lhs en %eax
        std::cout << "    cdq\n";              // étendre %eax vers %edx pour idiv
        std::cout << "    idivl %ecx\n";      // diviser par rhs
    }

    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx)
{
    visit(ctx->rhs());              // résultat dans %eax
    std::cout << "    negl %eax\n"; // %eax = -%eax
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx)
{
    return visit(ctx->rhs());
}