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
    // TODO: implémenter la logique
    // Label global (différent selon macOS / Linux)
    // pushq %rbp
    // movq %rsp, %rbp
    // subq $stackSize, %rsp   (aligné sur 16 octets)
}

void X86BackEnd::generateEpilogue(std::ostream& out) {
    // TODO: implémenter la logique
    // movq %rbp, %rsp
    // popq %rbp
    // ret
}

// ═══════════════════════════════════════════════════════════════════
//  ARMBackEnd
// ═══════════════════════════════════════════════════════════════════

void ARMBackEnd::generateCode(std::ostream& out) {
    // TODO: implémenter la logique ARM
}
