#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "SymbolTable.h"
#include "IR.h"

class IRControlFlowGraph;

class IRBasicBloc {
public:
    IRBasicBloc(IRControlFlowGraph* cfg, const std::string& label);

    ~IRBasicBloc();

    const std::string& getLabel() const { return label; }
    IRControlFlowGraph* getCFG() const { return cfg; }

    void addInstruction(IRInstruction* instr);

    void setExitTrue(IRBasicBloc* bloc)  { exitTrue = bloc; }
    void setExitFalse(IRBasicBloc* bloc) { exitFalse = bloc; }
    void setTestVarName(const std::string& name) { testVarName = name; }

    IRBasicBloc* getExitTrue() const  { return exitTrue; }
    IRBasicBloc* getExitFalse() const { return exitFalse; }
    const std::string& getTestVarName() const { return testVarName; }

    /// Génère le code x86 de toutes les instructions du bloc.
    void genX86(std::ostream& out) const;

    /// Génère le code ARM de toutes les instructions du bloc.
    void genARM(std::ostream& out) const;

    /// Affiche les instructions du bloc (debug).
    void printDebug(std::ostream& out) const;

private:
    std::string label;
    IRControlFlowGraph* cfg; // CFG parent
    std::vector<IRInstruction*> instructions;

    IRBasicBloc* exitTrue = nullptr;   // Branche si vrai (ou inconditionnelle)
    IRBasicBloc* exitFalse = nullptr;  // Branche si faux (nullable)
    std::string testVarName;           // Variable testée si exitFalse != nullptr
};

class IRControlFlowGraph {
public:
    IRControlFlowGraph(SymbolTable* symbolTable);

    ~IRControlFlowGraph();

    /// Ajoute un bloc au CFG et retourne un pointeur vers celui-ci.
    IRBasicBloc* addBasicBloc(const std::string& label);
    IRBasicBloc* addBasicBlocUnique(const std::string& prefix);

    IRBasicBloc* getCurrentBasicBloc() const { return currentBasicBloc; }

    void setCurrentBasicBloc(IRBasicBloc* bloc) { currentBasicBloc = bloc; }

    SymbolTable* getSymbolTable() const { return symbolTable; }

    std::string newTemp(const std::string& type = "int");

    void genX86(std::ostream& out) const;

    void genARM(std::ostream& out) const;

    void printDebug(std::ostream& out) const;

    const std::vector<IRBasicBloc*>& getBlocs() const { return blocs; }

private:
    std::vector<IRBasicBloc*> blocs;
    IRBasicBloc* currentBasicBloc = nullptr;  // Bloc en cours de construction
    SymbolTable* symbolTable;
    int nextBlocNumber = 0; // Pour générer des labels uniques
};
