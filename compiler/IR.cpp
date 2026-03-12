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
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl $" << value << ", " << offsetDest << "(%rbp)\n";
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
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetSrc << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    addl " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrAdd::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrSub
// ═══════════════════════════════════════════════════════════════════

IRInstrSub::IRInstrSub(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrSub::printDebug(std::ostream& out) const {
    out << "  sub " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrSub::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    subl " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrSub::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

