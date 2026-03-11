#include "CFG.h"

// ═══════════════════════════════════════════════════════════════════
//  IRBasicBloc
// ═══════════════════════════════════════════════════════════════════

IRBasicBloc::IRBasicBloc(IRControlFlowGraph* cfg, const std::string& label)
    : cfg(cfg), label(label) {}

IRBasicBloc::~IRBasicBloc() {
    for (auto* instr : instructions) {
        delete instr;
    }
}

void IRBasicBloc::addInstruction(IRInstruction* instr) {
    instructions.push_back(instr);
}

void IRBasicBloc::genX86(std::ostream& out) const {
    // TODO: implémenter la logique
    // 1. Écrire le label du bloc
    // 2. Appeler genX86() sur chaque instruction
    // 3. Gérer les sauts conditionnels / inconditionnels vers exit_true / exit_false
}

void IRBasicBloc::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

void IRBasicBloc::printDebug(std::ostream& out) const {
    out << label << ":\n";
    for (const auto* instr : instructions) {
        instr->printDebug(out);
    }
    if (exitTrue != nullptr) {
        out << "  -> " << exitTrue->getLabel();
        if (exitFalse != nullptr) {
            out << " (if " << testVarName << ") else " << exitFalse->getLabel();
        }
        out << "\n";
    }
}

// ═══════════════════════════════════════════════════════════════════
//  IRControlFlowGraph
// ═══════════════════════════════════════════════════════════════════

IRControlFlowGraph::IRControlFlowGraph(SymbolTable* symbolTable)
    : symbolTable(symbolTable) {}

IRControlFlowGraph::~IRControlFlowGraph() {
    for (auto* bloc : blocs) {
        delete bloc;
    }
}

IRBasicBloc* IRControlFlowGraph::addBasicBloc(const std::string& label) {
    auto* bloc = new IRBasicBloc(this, label);
    blocs.push_back(bloc);

    // Le premier bloc ajouté devient le bloc courant
    if (currentBasicBloc == nullptr) {
        currentBasicBloc = bloc;
    }

    return bloc;
}

std::string IRControlFlowGraph::newTemp(const std::string& type) {
    return symbolTable->newTemp(type);
}

void IRControlFlowGraph::genX86(std::ostream& out) const {
    // TODO: implémenter la logique
    // Parcourir les blocs et appeler genX86() sur chacun
    for (const auto* bloc : blocs) {
        bloc->genX86(out);
    }
}

void IRControlFlowGraph::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
    for (const auto* bloc : blocs) {
        bloc->genARM(out);
    }
}

void IRControlFlowGraph::printDebug(std::ostream& out) const {
    out << "=== CFG ===\n";
    for (const auto* bloc : blocs) {
        bloc->printDebug(out);
    }
    out << "===========\n";
}
