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

//  IRInstrSub : dest = lhs - rhs
//  Exemple IR :  sub dest lhs rhs
class IRInstrSub : public IRInstruction {
public:
    IRInstrSub(IRBasicBloc* parentBloc,
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

//  IRInstrMult : dest = lhs * rhs
//  Exemple IR :  mult dest lhs rhs
class IRInstrMult : public IRInstruction {
public:
    IRInstrMult(IRBasicBloc* parentBloc,
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

//  IRInstrDiv : dest = lhs / rhs
//  Exemple IR :  div dest lhs rhs
class IRInstrDiv : public IRInstruction {
public:
    IRInstrDiv(IRBasicBloc* parentBloc,
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

//  IRInstrMod : dest = lhs % rhs
//  Exemple IR :  mod dest lhs rhs
class IRInstrMod : public IRInstruction {
public:
    IRInstrMod(IRBasicBloc* parentBloc,
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

//  IRInstrCmp : dest = (lhs cmp rhs)
//  Exemple IR : cmp dest lhs rhs lt
class IRInstrCmp : public IRInstruction {
public:
    enum CmpType {
        LT,
        LE,
        GT,
        GE,
        EQ,
        NEQ
    };

    IRInstrCmp(IRBasicBloc* parentBloc,
               const std::string& dest,
               const std::string& lhs,
               const std::string& rhs,
               CmpType cmpType);

    void printDebug(std::ostream& out) const override;
    void genX86(std::ostream& out) const override;
    void genARM(std::ostream& out) const override;

private:
    std::string dest;
    std::string lhs;
    std::string rhs;
    CmpType cmpType;
};

//  IRInstrXor : dest = lhs ^ rhs
//  Exemple IR : xor dest lhs rhs
class IRInstrXor : public IRInstruction {
public:
    IRInstrXor(IRBasicBloc* parentBloc,
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

//  IRInstrOr : dest = lhs | rhs
//  Exemple IR : or dest lhs rhs
class IRInstrOr : public IRInstruction {
public:
    IRInstrOr(IRBasicBloc* parentBloc,
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


//  IRInstrAnd : dest = lhs & rhs
//  Exemple IR : and dest lhs rhs
class IRInstrAnd : public IRInstruction {
public:
    IRInstrAnd(IRBasicBloc* parentBloc,
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
