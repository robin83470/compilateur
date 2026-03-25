#pragma once

#include "antlr4-runtime.h"
#include "generated/ifccBaseVisitor.h"

#include "SymbolTable.h"
#include "CFG.h"

//  IRVisitor : visiteur ANTLR qui construit l'IR
//  1. Le main crée un IRVisitor
//  2. On appelle buildIr(tree) qui retourne un ControlFlowGraph (CFG) prêt
//  3. On passe le CFG à un BackEnd pour générer l'assembleur
class IRVisitor : public ifccBaseVisitor {
public:
    IRVisitor(SymbolTable* symbolTable);

    IRControlFlowGraph* buildIr(antlr4::tree::ParseTree* tree);


    const std::vector<std::pair<IRControlFlowGraph*, SymbolTable*>>& getAllFunctions() const {
        return allFunctions;
    }

    // ── Visiteurs ANTLR ─────────────────────────────────────────
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitFunction(ifccParser::FunctionContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclarator(ifccParser::DeclaratorContext* ctx) override;
    virtual antlrcpp::Any visitPointer_prefix(ifccParser::Pointer_prefixContext* ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) override;
    virtual antlrcpp::Any visitLvalue_id(ifccParser::Lvalue_idContext* ctx) override;
    virtual antlrcpp::Any visitLvalue_deref(ifccParser::Lvalue_derefContext* ctx) override;
    virtual antlrcpp::Any visitLvalue_parenthese(ifccParser::Lvalue_parentheseContext* ctx) override;
    virtual antlrcpp::Any visitBreak_stmt(ifccParser::Break_stmtContext* ctx) override;
    virtual antlrcpp::Any visitContinue_stmt(ifccParser::Continue_stmtContext* ctx) override;

    virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext* ctx) override;
    virtual antlrcpp::Any visitExpr_char(ifccParser::Expr_charContext* ctx) override;
    virtual antlrcpp::Any visitExpr_id(ifccParser::Expr_idContext* ctx) override;
    virtual antlrcpp::Any visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext* ctx) override;
    virtual antlrcpp::Any visitExpr_multdiv(ifccParser::Expr_multdivContext* ctx) override;
    virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_parenthese(ifccParser::Expr_parentheseContext* ctx) override;
    virtual antlrcpp::Any visitExpr_addrof(ifccParser::Expr_addrofContext* ctx) override;
    virtual antlrcpp::Any visitExpr_deref(ifccParser::Expr_derefContext* ctx) override;
    virtual antlrcpp::Any visitExpr_comparison(ifccParser::Expr_comparisonContext* ctx) override;
    virtual antlrcpp::Any visitExpr_equality(ifccParser::Expr_equalityContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitIf_stmt(ifccParser::If_stmtContext *ctx) override;
    virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext* ctx) override;
    virtual antlrcpp::Any visitExpr_getchar(ifccParser::Expr_getcharContext* ctx) override;
    virtual antlrcpp::Any visitExpr_putchar(ifccParser::Expr_putcharContext* ctx) override;
    virtual antlrcpp::Any visitExpr_funcCall(ifccParser::Expr_funcCallContext* ctx) override;
    virtual antlrcpp::Any visitRhsList(ifccParser::RhsListContext* ctx) override;
private:
    std::vector<std::pair<IRControlFlowGraph*, SymbolTable*>> allFunctions;
    struct LoopContext {
        IRBasicBloc* continueNextBlock; // bloc où sauter après le continue
        IRBasicBloc* breakNextBlock;    // bloc où sauter après le break
    };

    IRBasicBloc* createDeadBlock(const std::string& prefix); // Crée un bloc qui sera sauté (pour les break/continue)

    IRBasicBloc* epilogueBloc = nullptr;  //pointeur vers le bloc épilogue, pour y ajouter les instructions de retour
    IRControlFlowGraph* currentCFG = nullptr;
    SymbolTable* symbolTable;
    int ifCounter = 0;
    std::vector<LoopContext> loopStack; // pile des contextes de boucles pour gérer les boucles
};
