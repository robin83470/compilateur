#include "IR.h"
#include "CFG.h"

IRInstruction::IRInstruction(IRBasicBloc* parentBloc)
    : parentBloc(parentBloc) {}
    
// ═══════════════════════════════════════════════════════════════════
//  IRInstrConst
// ═══════════════════════════════════════════════════════════════════
IRInstrConst::IRInstrConst(IRBasicBloc* parentBloc, const std::string& dest, int value)
    : IRInstruction(parentBloc), dest(dest), value(value) {}

void IRInstrConst::printDebug(std::ostream& out) const {
    out << "  const " << dest << " #" << value << "\n";
}

void IRInstrConst::genX86(std::ostream& out) const {
    // TODO: implémenter la logique
    // Récupérer l'offset de dest via parentBloc->getCFG()->getSymbolTable()
    // Générer : movl $value, offset(%rbp)
}

void IRInstrConst::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrCopy
// ═══════════════════════════════════════════════════════════════════
IRInstrCopy::IRInstrCopy(IRBasicBloc* parentBloc, const std::string& dest, const std::string& src)
    : IRInstruction(parentBloc), dest(dest), src(src) {}

void IRInstrCopy::printDebug(std::ostream& out) const {
    out << "  copy " << dest << " " << src << "\n";
}

void IRInstrCopy::genX86(std::ostream& out) const {
    // TODO: implémenter la logique
    // movl srcOffset(%rbp), %eax
    // movl %eax, destOffset(%rbp)
}

void IRInstrCopy::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrAdd
// ═══════════════════════════════════════════════════════════════════
IRInstrAdd::IRInstrAdd(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrAdd::printDebug(std::ostream& out) const {
    out << "  add " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrAdd::genX86(std::ostream& out) const {
    // TODO: implémenter la logique
    // movl lhsOffset(%rbp), %eax
    // addl rhsOffset(%rbp), %eax
    // movl %eax, destOffset(%rbp)
}

void IRInstrAdd::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}
