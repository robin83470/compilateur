#pragma once

#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

struct SymbolInfo {
    int index;
    bool declared;
    bool used;
    std::string type; // Pour l'instant int uniquement
    int size;
    std::string sourceName;
    std::string storageName;
};

class SymbolTable {
public:
    SymbolTable() = default;

    void enterScope(); //ouvrir un nouveau bloc {
    void exitScope(); //fermer le bloc courant  }

    /// Erreur si le symbole existe déjà.
    SymbolInfo& addSymbol(const std::string& name, const std::string& type = "int");

    /// Recherche un identifiant source dans les scopes actifs.
    SymbolInfo& resolveSymbol(const std::string& name);

    /// Recherche un nom interne utilisé par l'IR/backend.
    SymbolInfo& getStorageSymbol(const std::string& storageName);

    int getOffset(const std::string& storageName);

    /// Crée un nouveau temporaire (nom unique) et retourne son nom.
    std::string newTemp(const std::string& type = "int");

    /// Retourne la taille totale occupée par les variables (valeur positive).
    int getTotalSize() const { return -currentOffset; }

    void checkUnusedVariables() const;

    int getTypeSize(const std::string& type) const;
    int getTypeAlign(const std::string& type) const;
    const std::string& getType(const std::string& name) const;
    bool isPointerType(const std::string& type) const;

private:
    using Scope = std::map<std::string, SymbolInfo*>;

    // Pile de scopes actifs pour la résolution lexicale.
    std::vector<Scope> scopes;
    // Stockage persistant des symboles pour l'IR et le backend.
    std::vector<std::unique_ptr<SymbolInfo>> ownedSymbols;
    std::map<std::string, SymbolInfo*> storageSymbols;
    int currentOffset = 0;   // Prochain offset libre (décrémenté)
    int tempCounter = 0;
    int symbolCounter = 0;

    void ensureActiveScope() const;
    std::string makeStorageName(const std::string& name);
};
