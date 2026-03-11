#pragma once

#include <string>
#include <vector>
#include <iostream>

class IRBasicBloc;

// Classe Abstraite
class IRInstruction {
public:
    IRInstruction(IRBasicBloc* parentBloc);
    virtual ~IRInstruction() = default;

    virtual void printDebug(std::ostream& out) const = 0;

    virtual void genX86(std::ostream& out) const = 0;

    virtual void genARM(std::ostream& out) const = 0;

protected:
    IRBasicBloc* parentBloc;
};

//  IRInstrConst : dest = constante
//  Exemple IR :  const dest #42
class IRInstrConst : public IRInstruction {
public:
    IRInstrConst(IRBasicBloc* parentBloc, const std::string& dest, int value);

    void printDebug(std::ostream& out) const override;
    void genX86(std::ostream& out) const override;
    void genARM(std::ostream& out) const override;

private:
    std::string dest;
    int value;
};

//  IRInstrCopy : dest = src
//  Exemple IR :  copy dest src
class IRInstrCopy : public IRInstruction {
public:
    IRInstrCopy(IRBasicBloc* parentBloc, const std::string& dest, const std::string& src);

    void printDebug(std::ostream& out) const override;
    void genX86(std::ostream& out) const override;
    void genARM(std::ostream& out) const override;

private:
    std::string dest;
    std::string src;
};

//  IRInstrAdd : dest = lhs + rhs
//  Exemple IR :  add dest lhs rhs
class IRInstrAdd : public IRInstruction {
public:
    IRInstrAdd(IRBasicBloc* parentBloc,
               const std::string& dest,
               const std::string& lhs,
               const std::string& rhs);

    void printDebug(std::ostream& out) const override;
    void genX86(std::ostream& out) const override;
    void genARM(std::ostream& out) const override;

private:
    std::string dest;
    std::string lhs;
    std::string rhs;
};
