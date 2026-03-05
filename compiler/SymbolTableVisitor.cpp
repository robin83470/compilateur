#include "SymbolTableVisitor.h"
#include <iostream>

antlrcpp::Any SymbolTableVisitor::visitProg(ifccParser::ProgContext *ctx) {
    visitChildren(ctx);
    checkUnusedVariables();
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclaration_stmt(ifccParser::Declaration_stmtContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any SymbolTableVisitor::visitAssign_stmt(ifccParser::Assign_stmtContext *ctx) {
    std::string varName = ctx->ID()->getText();
    checkVariableUsed(varName);
    symbolTable[varName].used = true;
    
    // Vérifier les variables utilisées dans le rhs
    auto rhs = ctx->rhs();
    if (rhs->ID() != nullptr) {
        std::string rhsVar = rhs->ID()->getText();
        checkVariableUsed(rhsVar);
    }
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitReturn_stmt(ifccParser::Return_stmtContext *ctx) {
    auto rhs = ctx->rhs();
    if (rhs->ID() != nullptr) {
        std::string varName = rhs->ID()->getText();
        checkVariableUsed(varName);
    }
    
    return 0;
}

antlrcpp::Any SymbolTableVisitor::visitDeclarator(ifccParser::DeclaratorContext *ctx) {
    std::string varName = ctx->ID()->getText();
    
    if (symbolTable.count(varName) > 0) {
        std::cerr << "Erreur : la variable '" << varName << "' est déjà déclarée." << std::endl;
        exit(1); 
    }
    
    // Créer une nouvelle entrée dans la table des symboles
    currentOffset -= 4;
    SymbolInfo info;
    info.index = currentOffset;
    info.declared = true;
    info.used = false;
    
    symbolTable[varName] = info;
    
    // Vérifier si le déclarateur a une valeur d'initialisation
    if (ctx->rhs() != nullptr) {
        if (ctx->rhs()->ID() != nullptr) {
            std::string initVar = ctx->rhs()->ID()->getText();
            checkVariableUsed(initVar);
        }
        symbolTable[varName].used = true;
    }
    
    return 0;
}

void SymbolTableVisitor::checkVariableUsed(const std::string& varName) {
    if (symbolTable.count(varName) == 0) {
        std::cerr << "Erreur : la variable '" << varName << "' a été utilisée mais n'a pas été déclarée." << std::endl;
        exit(1);
    }
    
    symbolTable[varName].used = true;
}

void SymbolTableVisitor::checkUnusedVariables() {
    for (const auto& pair : symbolTable) {
        const std::string& varName = pair.first;
        const SymbolInfo& info = pair.second;
        
        if (!info.used) {
            std::cerr << "Avertissement : la variable '" << varName << "' a été déclarée mais n'est pas utilisée." << std::endl;
        }
    }
}