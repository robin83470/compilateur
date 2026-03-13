#include "SymbolTable.h"

void SymbolTable::addSymbol(const std::string& name, const std::string& type) {
    if (symbols.count(name) > 0) {
        std::cerr << "Erreur : le symbole '" << name << "' est déjà déclaré." << std::endl;
        exit(1);
    }

    int sz = 4; // Par défaut pour "int"
    currentOffset -= sz;

    SymbolInfo info;
    info.index = currentOffset;
    info.declared = true;
    info.used = false;
    info.type = type;
    info.size = sz;

    symbols[name] = info;
}

SymbolInfo& SymbolTable::getSymbol(const std::string& name) {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        std::cerr << "Erreur : le symbole '" << name << "' n'existe pas." << std::endl;
        exit(1);
    }
    return it->second;
}

int SymbolTable::getOffset(const std::string& name) {
    return getSymbol(name).index;
}

std::string SymbolTable::newTemp(const std::string& type) {
    std::string tmpName = "!tmp" + std::to_string(tempCounter++);
    addSymbol(tmpName, type);
    symbols[tmpName].used = true; // Les temporaires sont toujours considérés utilisés
    return tmpName;
}

void SymbolTable::checkUnusedVariables() const {
    for (const auto& pair : symbols) {
        // Ignorer les temporaires internes
        if (pair.first[0] == '!') continue;

        if (!pair.second.used) {
            std::cerr << "Avertissement : la variable '" << pair.first
                      << "' a été déclarée mais n'est pas utilisée." << std::endl;
        }
    }
}
