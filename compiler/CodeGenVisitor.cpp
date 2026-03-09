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
    visit(ctx->rhs(0));
    std::cout << "    pushq %rax\n";
    visit(ctx->rhs(1));
    std::cout << "    movl %eax, %ebx\n";
    std::cout << "    popq %rcx\n";

    std::string op = ctx->children[1]->getText();
    std::cout << "    movl %ecx, %eax\n";

    if(op == "*") {
        std::cout << "    imull %ebx, %eax\n";
    }
    else if(op == "/") {
        std::cout << "    cdq\n";
        std::cout << "    idivl %ebx\n";
    }
    else if (op == "%") {
        std::string label = "end_modulo_" + std::to_string(ctx->getStart()->getStartIndex());
        
        std::cout << "    cdq\n";
        std::cout << "    idivl %ebx\n";       // Reste dans %edx
        std::cout << "    testl %edx, %edx\n"; // On regarde si le reste est < 0
        std::cout << "    jns " << label << "\n"; // Si positif ou nul, on saute à la fin
        
        // Ajustement euclidien : si reste < 0, reste = reste + abs(diviseur)
        std::cout << "    movl %ebx, %ecx\n";
        std::cout << "    negl %ecx\n";        // %ecx = -ebx
        std::cout << "    cmovsl %ebx, %ecx\n"; // Si %ebx était négatif, on reprend %ebx (donc abs)
        std::cout << "    addl %ecx, %edx\n";  // Reste final
        
        std::cout << label << ":\n";
        std::cout << "    movl %edx, %eax\n";
    }


    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) {
    visit(ctx->rhs());        // calcule l'opérande dans %eax
    std::cout << "    negl %eax\n";   // %eax = -%eax
    return 0;
}

antlrcpp::Any CodeGenVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) {
    return visit(ctx->rhs());
}