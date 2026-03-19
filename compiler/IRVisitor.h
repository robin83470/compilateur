#pragma once

#include <unordered_map>

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

    // ── Visiteurs ANTLR ─────────────────────────────────────────
    virtual antlrcpp::Any visitProg(ifccParser::ProgContext* ctx) override;
    virtual antlrcpp::Any visitReturn_stmt(ifccParser::Return_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclaration_stmt(ifccParser::Declaration_stmtContext* ctx) override;
    virtual antlrcpp::Any visitDeclarator(ifccParser::DeclaratorContext* ctx) override;
    virtual antlrcpp::Any visitAssign_stmt(ifccParser::Assign_stmtContext* ctx) override;

    virtual antlrcpp::Any visitExpr_const(ifccParser::Expr_constContext* ctx) override;
    virtual antlrcpp::Any visitExpr_char(ifccParser::Expr_charContext* ctx) override;
    virtual antlrcpp::Any visitExpr_id(ifccParser::Expr_idContext* ctx) override;
    virtual antlrcpp::Any visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext* ctx) override;
    virtual antlrcpp::Any visitExpr_multdiv(ifccParser::Expr_multdivContext* ctx) override;
    virtual antlrcpp::Any visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext* ctx) override;
    virtual antlrcpp::Any visitExpr_parenthese(ifccParser::Expr_parentheseContext* ctx) override;
    virtual antlrcpp::Any visitExpr_comparison(ifccParser::Expr_comparisonContext* ctx) override;
    virtual antlrcpp::Any visitExpr_and(ifccParser::Expr_andContext* ctx) override;
    virtual antlrcpp::Any visitExpr_or(ifccParser::Expr_orContext* ctx) override;
    virtual antlrcpp::Any visitExpr_xor(ifccParser::Expr_xorContext* ctx) override;
    virtual antlrcpp::Any visitIf_elsifelse(ifccParser::If_elsifelseContext *ctx) override;
    virtual antlrcpp::Any visitWhile_stmt(ifccParser::While_stmtContext* ctx) override;
    virtual antlrcpp::Any visitExpr_getchar(ifccParser::Expr_getcharContext* ctx) override;
    virtual antlrcpp::Any visitExpr_putchar(ifccParser::Expr_putcharContext* ctx) override;
    virtual antlrcpp::Any visitExpr_funcCall(ifccParser::Expr_funcCallContext* ctx) override;
    virtual antlrcpp::Any visitRhsList(ifccParser::RhsListContext* ctx) override;

    virtual antlrcpp::Any visitBlock(ifccParser::BlockContext* ctx) override;
    
private:
    IRBasicBloc* epilogueBloc = nullptr;  //pointeur vers le bloc épilogue, pour y ajouter les instructions de retour
    IRControlFlowGraph* currentCFG = nullptr;
    SymbolTable* symbolTable;
};
