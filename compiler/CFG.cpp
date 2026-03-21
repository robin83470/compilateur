#include "CFG.h"
#include "ArmCodegenUtils.h"

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
    // Label du bloc
    out << label << ":\n";

    // Générer le code de chaque instruction
    for (const auto* instr : instructions) {
        instr->genX86(out);
    }

    // Gestion des sauts
    if (exitFalse != nullptr) {
        // Saut conditionnel : tester testVarName
        int offsetTest = cfg->getSymbolTable()->getOffset(testVarName);
        out << "    cmpl $0, " << offsetTest << "(%rbp)\n";
        out << "    je " << exitFalse->getLabel() << "\n";
        out << "    jmp " << exitTrue->getLabel() << "\n";
    } else if (exitTrue != nullptr) {
        // Saut inconditionnel
        out << "    jmp " << exitTrue->getLabel() << "\n";
    }
}

void IRBasicBloc::genARM(std::ostream& out) const {
    out << label << ":\n";

    for (const auto* instr : instructions) {
        instr->genARM(out);
    }

    if (exitFalse != nullptr) {
        int offsetTest = cfg->getSymbolTable()->getOffset(testVarName);
        arm_codegen::emitLoadWFromOffset(out, offsetTest, "w9");
        out << "    cmp w9, #0\n";
        out << "    beq " << exitFalse->getLabel() << "\n";
        out << "    b " << exitTrue->getLabel() << "\n";
    } else if (exitTrue != nullptr) {
        out << "    b " << exitTrue->getLabel() << "\n";
    }
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
    // Parcourir les blocs et appeler genX86() sur chacun
    // On exclut le bloc exit car le BackEnd le gère
    for (const auto* bloc : blocs) {
        if (bloc->getLabel().find("_exit") != std::string::npos) continue;
        bloc->genX86(out);
    }
}

void IRControlFlowGraph::genARM(std::ostream& out) const {
    for (const auto* bloc : blocs) {
        if (bloc->getLabel().find("_exit") != std::string::npos) continue;
        bloc->genARM(out);
    }
}
