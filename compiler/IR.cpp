#include "IR.h"
#include "CFG.h"
#include "ArmCodegenUtils.h"

#include <stdexcept>

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

const char* cmpTypeToArmCondition(IRInstrCmp::CmpType cmpType) {
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
            return "ne";
    }
    throw std::runtime_error("Unknown comparison type");
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
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadImm32(out, "w9", value);
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w9");
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
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetSrc, "w9");
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w9");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    add w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    sub w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    mul w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    sdiv w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    sdiv w11, w9, w10\n";
    out << "    msub w11, w11, w10, w9\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    cmp w9, w10\n";
    out << "    cset w11, " << cmpTypeToArmCondition(cmpType) << "\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    eor w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    orr w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetLhs = parentBloc->getCFG()->getSymbolTable()->getOffset(lhs);
    int offsetRhs = parentBloc->getCFG()->getSymbolTable()->getOffset(rhs);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetLhs, "w9");
    arm_codegen::emitLoadWFromOffset(out, offsetRhs, "w10");
    out << "    and w11, w9, w10\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w11");
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
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetSrc, "w9");
    out << "    neg w10, w9\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w10");
}

// ═══════════════════════════════════════════════════════════════════
// IRInstrPutchar
// ═══════════════════════════════════════════════════════════════════

IRInstrPutchar::IRInstrPutchar(IRBasicBloc* parentBloc, const std::string& dest, const std::string& src)
    : IRInstruction(parentBloc), dest(dest), src(src) {}

void IRInstrPutchar::printDebug(std::ostream& out) const {
    out << "  putchar " << dest << " " << src << "\n";
}

void IRInstrPutchar::genX86(std::ostream& out) const {
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl " << offsetSrc << "(%rbp), %edi\n"; // on met l'argument dans %edi
    out << "    callq _putchar\n"; // appel à putchar
    out << "    movl %eax, " << offsetDest << "(%rbp)\n"; // stocker la valeur de retour
}

void IRInstrPutchar::genARM(std::ostream& out) const {
    int offsetSrc = parentBloc->getCFG()->getSymbolTable()->getOffset(src);
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    arm_codegen::emitLoadWFromOffset(out, offsetSrc, "w0");
    out << "    bl _putchar\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w0");
}

// ═══════════════════════════════════════════════════════════════════
// IRInstrGetchar
// ═══════════════════════════════════════════════════════════════════

IRInstrGetchar::IRInstrGetchar(IRBasicBloc* parentBloc, const std::string& dest)
    : IRInstruction(parentBloc), dest(dest) {}

void IRInstrGetchar::printDebug(std::ostream& out) const {
    out << "  getchar " << dest << "\n";
}

void IRInstrGetchar::genX86(std::ostream& out) const {
    out << "    callq _getchar\n"; // appel à getchar
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    movl %eax, " << offsetDest << "(%rbp)\n"; // stocker la valeur de retour
}

void IRInstrGetchar::genARM(std::ostream& out) const {
    int offsetDest = parentBloc->getCFG()->getSymbolTable()->getOffset(dest);
    out << "    bl _getchar\n";
    arm_codegen::emitStoreWToOffset(out, offsetDest, "w0");
}
