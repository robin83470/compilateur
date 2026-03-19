#include "SymbolTable.h"
#include <stdexcept>

// Fonctions utilitaires nécessaires à la gestion des types
static std::string normalizeType(const std::string& t) {
    std::string out;
    out.reserve(t.size());
    for (char c : t) {
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') out.push_back(c);
    }
    return out;
}

static int pointerDepth(const std::string& rawType) {
    std::string t = normalizeType(rawType);
    if (t.size() < 3 || t.rfind("int", 0) != 0) return -1;
    for (size_t i = 3; i < t.size(); ++i) {
        if (t[i] != '*') return -1;
    }
    return static_cast<int>(t.size() - 3);
}

static int alignDownNegOffset(int value, int align) {
    if (align <= 0) return value;
    int absV = -value;
    int rem = absV % align;
    if (rem == 0) return value;
    return -(absV + (align - rem));
}

void SymbolTable::ensureActiveScope() const {
    if (scopes.empty()) {
        throw std::runtime_error("internal error: no active scope");
    }
}

std::string SymbolTable::makeStorageName(const std::string& name) {
    if (!name.empty() && name[0] == '!') {
        return name;
    }
    return name + "#" + std::to_string(symbolCounter++);
}

void SymbolTable::enterScope() {
    //Ajoute une map vide (nouveau scope) au sommet de la pile
    scopes.push_back({});
}

void SymbolTable::exitScope() {
    ensureActiveScope();

    //vérifier les variables non utilisées dans le scope 
    for (const auto& pair : scopes.back()) {
        const SymbolInfo& info = *pair.second;
        if (!info.sourceName.empty() && info.sourceName[0] == '!') continue; // ignorer les variables temporaires
        if (!info.used) {
            std::cerr << "Avertissement : la variable '" << info.sourceName
                      << "' a été déclarée mais n'est pas utilisée." << std::endl;
        }
    }
    //détruire la portée 
    scopes.pop_back();    
}

SymbolInfo& SymbolTable::addSymbol(const std::string& name, const std::string& type) {
    ensureActiveScope();

    // Vérifie seulement dans le scope COURANT (on a le droit de shadower un scope parent)
    if (scopes.back().count(name) > 0) {
        std::cerr << "Erreur : le symbole '" << name << "' est déjà déclaré." << std::endl;
        exit(1);
    }

    int sz = getTypeSize(type);
    int al = getTypeAlign(type);

    currentOffset = alignDownNegOffset(currentOffset - sz, al);


    auto info = std::make_unique<SymbolInfo>();
    info->index = currentOffset;
    info->declared = true;
    info->used = false;
    info->type = type;
    info->size = sz;
    info->sourceName = name;
    info->storageName = makeStorageName(name);


    SymbolInfo* infoPtr = info.get();
    scopes.back()[name] = infoPtr;
    storageSymbols[infoPtr->storageName] = infoPtr;
    ownedSymbols.push_back(std::move(info));

    return *infoPtr;
}

SymbolInfo& SymbolTable::resolveSymbol(const std::string& name) {
    ensureActiveScope();

    // Cherche du scope le plus interne (back) vers le plus externe (front)
    for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; i--) {
        auto it = scopes[i].find(name);
        if (it != scopes[i].end()) {
            return *it->second;
        }
    }
    std::cerr << "Erreur : le symbole '" << name << "' n'existe pas." << std::endl;
    exit(1);
    throw std::runtime_error("unreachable");
}

SymbolInfo& SymbolTable::getStorageSymbol(const std::string& storageName) {
    auto it = storageSymbols.find(storageName);
    if (it != storageSymbols.end()) {
        return *it->second;
    }

    std::cerr << "Erreur : le symbole interne '" << storageName << "' n'existe pas." << std::endl;
    exit(1);
    throw std::runtime_error("unreachable");
}

int SymbolTable::getOffset(const std::string& storageName) {
    return getStorageSymbol(storageName).index;
}

std::string SymbolTable::newTemp(const std::string& type) {
    std::string tmpName = "!tmp" + std::to_string(tempCounter++);
    SymbolInfo& info = addSymbol(tmpName, type);
    info.used = true; // Les temporaires sont toujours considérés utilisés
    return info.storageName;
}

void SymbolTable::checkUnusedVariables() const {
    for (const auto& symbol : ownedSymbols) {
        if (!symbol->sourceName.empty() && symbol->sourceName[0] == '!') continue;
        if (!symbol->used) {
            std::cerr << "Avertissement : la variable '" << symbol->sourceName
                      << "' a été déclarée mais n'est pas utilisée." << std::endl;
        }
    }
}

int SymbolTable::getTypeSize(const std::string& type) const {
    std::string normType = normalizeType(type);
    int ptrDepth = pointerDepth(type);
    if (ptrDepth > 0) {
        return 8; // Taille d'un pointeur
    }
    if (normType == "int") {
        return 4;
    }
    std::cerr << "Erreur : type inconnu '" << type << "'." << std::endl;
    exit(1);
}

int SymbolTable::getTypeAlign(const std::string& type) const {
    std::string normType = normalizeType(type);
    int ptrDepth = pointerDepth(type);
    if (ptrDepth > 0) {
        return 8; // Alignement d'un pointeur
    }
    if (normType == "int") {
        return 4;
    }
    std::cerr << "Erreur : type inconnu '" << type << "'." << std::endl;
    exit(1);
}

const std::string& SymbolTable::getType(const std::string& name) const {
    auto it = symbols.find(name);
    if (it == symbols.end()) {
        std::cerr << "Erreur : le symbole '" << name << "' n'existe pas." << std::endl;
        exit(1);
    }
    return it->second.type;
}

bool SymbolTable::isPointerType(const std::string& type) const {
    return pointerDepth(type) > 0;
}