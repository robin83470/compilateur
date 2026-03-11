#pragma once

#include <map>
#include <string>
#include <iostream>

struct SymbolInfo {
    int index;
    bool declared;
    bool used;
    std::string type; // Pour l'instant int uniquement
    int size;
};

class SymbolTable {
public:
    SymbolTable() = default;

    /// Erreur si le symbole existe déjà.
    void addSymbol(const std::string& name, const std::string& type = "int");

    /// Erreur si le symbole n'existe pas.
    SymbolInfo& getSymbol(const std::string& name);

    int getOffset(const std::string& name);

    /// Crée un nouveau temporaire (nom unique) et retourne son nom.
    std::string newTemp(const std::string& type = "int");

    void checkUnusedVariables() const;

    const std::map<std::string, SymbolInfo>& getMap() const { return symbols; }

private:
    std::map<std::string, SymbolInfo> symbols;
    int currentOffset = 0;   // Prochain offset libre (décrémenté)
    int tempCounter = 0;
};
