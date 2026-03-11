#pragma once

#include <iostream>
#include "CFG.h"

//  Classe abstraite pour la génération de code final
class BackEnd {
public:
    explicit BackEnd(IRControlFlowGraph* cfg) : cfg(cfg) {}
    virtual ~BackEnd() = default;

    virtual void generateCode(std::ostream& out) = 0;

protected:
    IRControlFlowGraph* cfg;
};

// Code x86
class X86BackEnd : public BackEnd {
public:
    explicit X86BackEnd(IRControlFlowGraph* cfg) : BackEnd(cfg) {}

    void generateCode(std::ostream& out) override;

private:
    void generatePrologue(std::ostream& out);
    void generateEpilogue(std::ostream& out);
};

// Code ARM
class ARMBackEnd : public BackEnd {
public:
    explicit ARMBackEnd(IRControlFlowGraph* cfg) : BackEnd(cfg) {}

    void generateCode(std::ostream& out) override;
};
