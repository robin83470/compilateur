#include "SymbolTableVisitor.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {
std::string normalizeType(const std::string& type) {
    // Enlève les espaces et les tabulations pour comparer les types de manière plus flexible
    std::string out;
    out.reserve(type.size());
    for (char c : type) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            out.push_back(c);
        }
    }
    return out;
}

bool isIntType(const std::string& type) {
    return normalizeType(type) == "int";
}
}

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx) {
    // Visite tous les enfants
    for (auto* func : ctx->function()) {
        if (func->ID()) {
            knownFunctions.push_back(func->ID()->getText());
        }
    }
    visitChildren(ctx);
    symbolTable.checkUnusedVariables();
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitPointer_prefix(ifccParser::Pointer_prefixContext *ctx) {
    // Détermine la profondeur de pointeur (ex: int** -> 2)
    if (ctx->pointer_prefix() == nullptr) {
        return 1;
    }

    int subDepth = std::any_cast<int>(visit(ctx->pointer_prefix()));
    return subDepth + 1;
}


antlrcpp::Any SymbolTableVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    // Vérifie que le type de l'expression à droite est compatible avec le type de la variable à gauche
    std::string lhsType = std::any_cast<std::string>(visit(ctx->lvalue()));
    std::string rhsType = std::any_cast<std::string>(visit(ctx->rhs()));
    requireType(rhsType, lhsType, "assignation");

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitLvalue_id(ifccParser::Lvalue_idContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);
    return symbolTable.getType(varName);
}

antlrcpp::Any SymbolTableVisitor::visitLvalue_deref(ifccParser::Lvalue_derefContext *ctx) {
    std::string nestedType = std::any_cast<std::string>(visit(ctx->lvalue()));
    return removePointerLevel(nestedType, "deref lvalue");
}

antlrcpp::Any SymbolTableVisitor::visitLvalue_parenthese(ifccParser::Lvalue_parentheseContext *ctx) {
    return visit(ctx->lvalue());
}

antlrcpp::Any SymbolTableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    std::string returnType = std::any_cast<std::string>(visit(ctx->rhs()));
    requireType(returnType, "int", "return");

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx) {
    std::string varName = ctx->ID()->getText();
    int pointerDepth = 0;
    if (ctx->pointer_prefix() != nullptr) {
        pointerDepth = std::any_cast<int>(visit(ctx->pointer_prefix()));
    }

    std::string varType = "int" + std::string(pointerDepth, '*');
    // Crée le symbole dans la table des symboles. Par exe : int** x; -> type = "int**"

    symbolTable.addSymbol(varName, varType);  // Erreur automatique si déjà déclaré

    if (ctx->rhs() != nullptr) {
        std::string initType = std::any_cast<std::string>(visit(ctx->rhs()));
        requireType(initType, varType, "initialisation de '" + varName + "'");
        symbolTable.getSymbol(varName).used = true;
    }

    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_id(ifccParser::Expr_idContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);
    return symbolTable.getType(varName);
}

antlrcpp::Any SymbolTableVisitor::visitExpr_const(ifccParser::Expr_constContext *ctx) {
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_char(ifccParser::Expr_charContext *ctx) {
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_parenthese(ifccParser::Expr_parentheseContext *ctx) {
    return visit(ctx->rhs());
}

antlrcpp::Any SymbolTableVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext *ctx) {
    // Vérifie que lhs et rhs sont de type int et retourne int
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de +/-");
    requireType(rhs, "int", "operande droit de +/-");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_multdiv(ifccParser::Expr_multdivContext *ctx) {
    // Vérifie que lhs et rhs sont de type int et retourne int
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de */%");
    requireType(rhs, "int", "operande droit de */%");
    return std::string("int");
}

//  Prochaines méthodes : vérifient que lhs et rhs sont de type int et retournent int

antlrcpp::Any SymbolTableVisitor::visitExpr_moinsunaire(ifccParser::Expr_moinsunaireContext *ctx) {
    std::string operandType = std::any_cast<std::string>(visit(ctx->rhs()));
    requireType(operandType, "int", "operateur unaire");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_comparison(ifccParser::Expr_comparisonContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de comparaison");
    requireType(rhs, "int", "operande droit de comparaison");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_equality(ifccParser::Expr_equalityContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(rhs, lhs, "equality");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_and(ifccParser::Expr_andContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de & binaire");
    requireType(rhs, "int", "operande droit de & binaire");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_xor(ifccParser::Expr_xorContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de ^");
    requireType(rhs, "int", "operande droit de ^");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_or(ifccParser::Expr_orContext *ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    requireType(lhs, "int", "operande gauche de |");
    requireType(rhs, "int", "operande droit de |");
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_getchar(ifccParser::Expr_getcharContext *ctx) {
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_putchar(ifccParser::Expr_putcharContext *ctx) {
    // Vérifie que l'argument de putchar est de type int
    auto* ioArg = ctx->io_arg();
    if (ioArg->ID()) {
        std::string varName = ioArg->ID()->getText();
        checkVariableUsed(varName);
        requireType(symbolTable.getType(varName), "int", "argument de putchar");
    }
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitExpr_addrof(ifccParser::Expr_addrofContext *ctx) {
    std::string baseType = std::any_cast<std::string>(visit(ctx->lvalue()));
    return addPointerLevel(baseType);
}

antlrcpp::Any SymbolTableVisitor::visitExpr_deref(ifccParser::Expr_derefContext *ctx) {
    std::string ptrType = std::any_cast<std::string>(visit(ctx->rhs()));
    return removePointerLevel(ptrType, "deref expression");
}

void SymbolTableVisitor::checkVariableUsed(const std::string& varName) {
    // getSymbol() sort en erreur si la variable n'existe pas
    symbolTable.getSymbol(varName).used = true;
}

antlrcpp::Any SymbolTableVisitor::visitExpr_funcCall(ifccParser::Expr_funcCallContext* ctx) {
    // Vérifier les types des arguments
    std::string funcName = ctx->ID()->getText();
    if (std::find(knownFunctions.begin(), knownFunctions.end(), funcName) == knownFunctions.end()) {
        throw std::runtime_error("Erreur : la fonction '" + funcName + "' n'est pas déclarée.");
    }
    if (ctx->rhsList()) {
        for (auto* rhs : ctx->rhsList()->rhs()) {
            visit(rhs);
        }
    }
    return std::string("int");
}

antlrcpp::Any SymbolTableVisitor::visitFunction(ifccParser::FunctionContext *ctx) {
    SymbolTable savedTable = symbolTable;  // sauvegarde
    symbolTable = SymbolTable{};           // table fraîche pour cette fonction
    if (ctx->paramList()) {
        auto* paramList = ctx->paramList();
        for (auto* id : paramList->ID()) {
            std::string paramName = id->getText();
            symbolTable.addSymbol(paramName);
            symbolTable.getSymbol(paramName).used = true;
        }
    }
    for (auto* stmt : ctx->stmt()) {
        visit(stmt);
    }

    symbolTable = savedTable;  // restaure
    return 0;
}

void SymbolTableVisitor::requireType(const std::string& actual, const std::string& expected, const std::string& where) {
    // Vérifie que les types sont les mêmes (après normalisation) et sort une erreur sinon
    if (normalizeType(actual) != normalizeType(expected)) {
        throw std::runtime_error(
            "Erreur de type (" + where + ") : attendu '" + expected + "', recu '" + actual + "'."
        );
    }
}

std::string SymbolTableVisitor::addPointerLevel(const std::string& type) const {
    return normalizeType(type) + "*";
}

std::string SymbolTableVisitor::removePointerLevel(const std::string& type, const std::string& where) const {
    // Renvoie le type pointé par un pointeur. Par exemple, "int**" -> "int*"
    std::string normalized = normalizeType(type);
    if (normalized.empty() || normalized.back() != '*') {
        throw std::runtime_error(
            "Erreur de type (" + where + ") : déréférencement d'une expression non pointeur ('" + type + "')."
        );
    }

    normalized.pop_back();
    return normalized;
}
