#include "SymbolTable.h"

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

void SymbolTable::addSymbol(const std::string& name, const std::string& type) {
    if (symbols.count(name) > 0) {
        std::cerr << "Erreur : le symbole '" << name << "' est déjà déclaré." << std::endl;
        exit(1);
    }

    int sz = getTypeSize(type);
    int al = getTypeAlign(type);

    currentOffset = alignDownNegOffset(currentOffset - sz, al);

    SymbolInfo info;
    info.index = currentOffset;
    info.declared = true;
    info.used = false;
    info.type = normalizeType(type);
    info.size = sz;
    info.align = al;

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
