#include "IR.h"
#include "CFG.h"

namespace {
const char* cmpTypeToString(IRInstrCmp::CmpType cmpType) {
    switch (cmpType) {
        case IRInstrCmp::LT:
            return "lt";
        case IRInstrCmp::LE:
            return "le";
        case IRInstrCmp::GT:
            return "gt";
        case IRInstrCmp::GE:
            return "ge";
        case IRInstrCmp::EQ:
            return "eq";
        case IRInstrCmp::NEQ:
            return "neq";
    }
    return "?";
}
}

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

// ═══════════════════════════════════════════════════════════════════
//  IRInstrMult
// ═══════════════════════════════════════════════════════════════════

IRInstrMult::IRInstrMult(IRBasicBloc* parentBloc,
                         const std::string& dest,
                         const std::string& lhs,
                         const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrMult::printDebug(std::ostream& out) const {
    out << "  mult " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrMult::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    imull " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrMult::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrDiv
// ═══════════════════════════════════════════════════════════════════

IRInstrDiv::IRInstrDiv(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrDiv::printDebug(std::ostream& out) const {
    out << "  div " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrDiv::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    cdq\n";
    out << "    idivl " << offsetRhs << "(%rbp)\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrDiv::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrMod
// ═══════════════════════════════════════════════════════════════════
IRInstrMod::IRInstrMod(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrMod::printDebug(std::ostream& out) const {
    out << "  mod " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrMod::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    cdq\n";
    out << "    idivl " << offsetRhs << "(%rbp)\n";
    out << "    movl %edx, " << offsetDest << "(%rbp)\n";
}

void IRInstrMod::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrCmp
// ═══════════════════════════════════════════════════════════════════

IRInstrCmp::IRInstrCmp(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs,
                       CmpType cmpType)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs), cmpType(cmpType) {}

void IRInstrCmp::printDebug(std::ostream& out) const {
    out << "  cmp " << dest << " " << lhs << " " << rhs << " " << cmpTypeToString(cmpType) << "\n";
}

void IRInstrCmp::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    cmpl " << offsetRhs << "(%rbp), %eax\n";
    switch (cmpType) {
        case LT:
            out << "    setl %al\n";
            break;
        case LE:
            out << "    setle %al\n";
            break;
        case GT:
            out << "    setg %al\n";
            break;
        case GE:
            out << "    setge %al\n";
            break;
        case EQ:
            out << "    sete %al\n";
            break;
        case NEQ:
            out << "    setne %al\n";
            break;
        default:
            throw std::runtime_error("Unknown comparison type");
    }
    out << "    movzbl %al, %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrCmp::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrXor
// ═══════════════════════════════════════════════════════════════════

IRInstrXor::IRInstrXor(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrXor::printDebug(std::ostream& out) const {
    out << "  xor " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrXor::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    xorl " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrXor::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrOr
// ═══════════════════════════════════════════════════════════════════

IRInstrOr::IRInstrOr(IRBasicBloc* parentBloc,
                     const std::string& dest,
                     const std::string& lhs,
                     const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrOr::printDebug(std::ostream& out) const {
    out << "  or " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrOr::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    orl " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrOr::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}


// ═══════════════════════════════════════════════════════════════════
//  IRInstrAnd
// ═══════════════════════════════════════════════════════════════════

IRInstrAnd::IRInstrAnd(IRBasicBloc* parentBloc,
                       const std::string& dest,
                       const std::string& lhs,
                       const std::string& rhs)
    : IRInstruction(parentBloc), dest(dest), lhs(lhs), rhs(rhs) {}

void IRInstrAnd::printDebug(std::ostream& out) const {
    out << "  and " << dest << " " << lhs << " " << rhs << "\n";
}

void IRInstrAnd::genX86(std::ostream& out) const {
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetLhs << "(%rbp), %eax\n";
    out << "    andl " << offsetRhs << "(%rbp), %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrAnd::genARM(std::ostream& out) const {
    // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
// IRInstrNeg
// ═══════════════════════════════════════════════════════════════════

IRInstrNeg::IRInstrNeg(IRBasicBloc* parentBloc, const std::string& dest, const std::string& src)
    : IRInstruction(parentBloc), dest(dest), src(src) {}

void IRInstrNeg::printDebug(std::ostream& out) const {
    out << "  neg " << dest << " " << src << "\n";
}

void IRInstrNeg::genX86(std::ostream& out) const {
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetSrc << "(%rbp), %eax\n";
    out << "    negl %eax\n";
    out << "    movl %eax, " << offsetDest << "(%rbp)\n";
}

void IRInstrNeg::genARM(std::ostream& out) const {
        // TODO: implémenter la logique
}

// ═══════════════════════════════════════════════════════════════════
//  IRInstrGetParam
// ═══════════════════════════════════════════════════════════════════

IRInstrGetParam::IRInstrGetParam(IRBasicBloc* parentBloc, const std::string& dest, int paramIndex)
    : IRInstruction(parentBloc), dest(dest), paramIndex(paramIndex) {}

void IRInstrGetParam::printDebug(std::ostream& out) const {
    out << "    getParam " << dest << " index " << paramIndex << "\n";
}

void IRInstrGetParam::genX86(std::ostream& out) const {
    // Registres 32 bits (pour int)
    static const std::vector<std::string> reg32 = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    // Registres 64 bits (pour pointeurs ou long)
    static const std::vector<std::string> reg64 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

    if (paramIndex < 6) {
        int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
        
        int size = parentBloc->getCFG()->getSymbolTable()->getTotalSize();


        if (size == 8) {
            out << "    movq " << reg64[paramIndex] << ", " << offsetDest << "(%rbp)\n";
        } else {
            out << "    movl " << reg32[paramIndex] << ", " << offsetDest << "(%rbp)\n";
        }
    } else {
        out << "    # Error: Parameter index " << paramIndex << " > 6 not supported\n";
    }
}

//
// IR Return
//
IRInstrRet::IRInstrRet(IRBasicBloc* parentBloc, const std::string& src)
    : IRInstruction(parentBloc), src(src) {}

void IRInstrRet::printDebug(std::ostream& out) const {
    out << "    ret " << src << "\n";
}

void IRInstrRet::genX86(std::ostream& out) const {
    int offset = parentBloc->getCFG()->getSymbolTable()->getOffset(src);

    out << "    movl " << offset << "(%rbp), %eax" << "\n";

    std::string funcName = parentBloc->getCFG()->getBlocs()[0]->getLabel();
    
    out << "    jmp ." << funcName << "_exit" << "\n";
}

