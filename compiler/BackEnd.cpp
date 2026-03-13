#include "BackEnd.h"

// ═══════════════════════════════════════════════════════════════════
//  X86BackEnd
// ═══════════════════════════════════════════════════════════════════

void X86BackEnd::generateCode(std::ostream& out) {
    generatePrologue(out);
    cfg->genX86(out);
    generateEpilogue(out);
}

void X86BackEnd::generatePrologue(std::ostream& out) {
#ifdef __APPLE__
    out << ".globl _main\n";
    out << "_main:\n";
#else
    out << ".globl main\n";
    out << "main:\n";
#endif
    out << "    pushq %rbp\n";
    out << "    movq %rsp, %rbp\n";

    // Calculer la taille de la pile alignée sur 16 octets
    int totalSize = cfg->getSymbolTable()->getTotalSize();
    int stackSize = ((totalSize + 15) / 16) * 16;
    if (stackSize > 0) {
        out << "    subq $" << stackSize << ", %rsp\n";
    }
}

void X86BackEnd::generateEpilogue(std::ostream& out) {
    // Charger la valeur de retour dans %eax
    int offsetRetval = cfg->getSymbolTable()->getOffset("!retval");
    out << "    movl " << offsetRetval << "(%rbp), %eax\n";

    out << "    movq %rbp, %rsp\n";
    out << "    popq %rbp\n";
    out << "    ret\n";
}

// ═══════════════════════════════════════════════════════════════════
//  ARMBackEnd
// ═══════════════════════════════════════════════════════════════════

void ARMBackEnd::generateCode(std::ostream& out) {
    // TODO: implémenter la logique ARM
}
