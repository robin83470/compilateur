# Architecture IR du compilateur — Guide pour l'équipe

## 1. AVANT vs MAINTENANT

### Ancien pipeline (CodeGenVisitor — supprimé)
```
Source C → ANTLR Parse → SymbolTableVisitor (passe 1) → CodeGenVisitor (passe 2)
                                                              ↓
                                                     Assembleur x86 sur stdout
```
Le CodeGenVisitor visitait l'AST et **émettait directement du x86** dans chaque méthode `visit*()`. Ça marchait mais c'était impossible à étendre (pas de support multi-cible, pas d'optimisation possible).

### Nouveau pipeline (3 passes)
```
Source C → ANTLR Parse → SymbolTableVisitor (passe 1 : validation)
                       → IRVisitor          (passe 2 : construit l'IR)
                       → X86BackEnd         (passe 3 : génère l'assembleur depuis l'IR)
```

**L'IR (Représentation Intermédiaire)** est une couche entre l'AST et l'assembleur. C'est un langage simple de "pseudo-instructions" qui ne dépend d'aucune architecture.

## 2. Les fichiers et leurs rôles

| Fichier | Rôle |
|---|---|
| `SymbolTable.h/.cpp` | Table des symboles. Stocke les variables et les temporaires avec leur offset mémoire |
| `SymbolTableVisitor.h/.cpp` | Passe 1 : parcourt l'AST pour déclarer les variables dans la SymbolTable et vérifier les erreurs (variable non déclarée, doublon, etc.) |
| `IR.h/.cpp` | Définit les instructions IR (IRInstrConst, IRInstrCopy, IRInstrAdd, ...). Chaque instruction sait se printer (`printDebug`) et se traduire en assembleur (`genX86`) |
| `CFG.h/.cpp` | IRBasicBloc (= un bloc d'instructions consécutives) et IRControlFlowGraph (= le graphe de tous les blocs d'une fonction) |
| `IRVisitor.h/.cpp` | Passe 2 : parcourt l'AST et **génère des instructions IR** dans le CFG |
| `BackEnd.h/.cpp` | Passe 3 : prend un CFG et produit l'assembleur final (prologue → blocs → épilogue) |
| `main.cpp` | Branche les 3 passes ensemble |

## 3. Exemple concret pas à pas

### Programme C
```c
int main() {
    int a = 2;
    int b = 3;
    int c = a + b;
    return c;
}
```

### Passe 1 — SymbolTableVisitor
Remplit la SymbolTable :
```
a      → offset -4,  type int, size 4
b      → offset -8,  type int, size 4
c      → offset -12, type int, size 4
```

### Passe 2 — IRVisitor
Parcourt l'AST et génère des instructions IR. Voici ce qui se passe visiteur par visiteur :

**`visitDeclarator("int a = 2")`** :
1. Visite l'expression `2` → appelle `visitExpr_const`
2. `visitExpr_const` crée un temporaire `!tmp0` et émet `const !tmp0 #2`
3. Retourne au declarator qui émet `copy a !tmp0`

**`visitDeclarator("int b = 3")`** : même chose → `const !tmp1 #3` puis `copy b !tmp1`

**`visitDeclarator("int c = a + b")`** :
1. Visite `a + b` → appelle `visitExpr_plusmoins`
2. `visitExpr_plusmoins` visite `a` (retourne `"a"`) et `b` (retourne `"b"`)
3. Crée un temporaire `!tmp2` et émet `add !tmp2 a b`
4. Retourne au declarator qui émet `copy c !tmp2`

**`visitReturn_stmt("return c")`** :
1. Visite `c` → `visitExpr_id` retourne `"c"`
2. Émet `copy !retval c`

### IR résultant (affiché par `printDebug`)
```
.entry:
  const !tmp0 #2
  copy a !tmp0
  const !tmp1 #3
  copy b !tmp1
  add !tmp2 a b
  copy c !tmp2
  copy !retval c
```

### Passe 3 — X86BackEnd
Le BackEnd traduit chaque instruction IR en x86 :

```asm
.globl _main
_main:
    pushq %rbp                    ; prologue (BackEnd)
    movq %rsp, %rbp
    subq $32, %rsp
.entry:                           ; bloc (CFG)
    movl $2, -16(%rbp)            ; const !tmp0 #2
    movl -16(%rbp), %eax          ; copy a !tmp0
    movl %eax, -4(%rbp)
    movl $3, -20(%rbp)            ; const !tmp1 #3
    movl -20(%rbp), %eax          ; copy b !tmp1
    movl %eax, -8(%rbp)
    movl -4(%rbp), %eax           ; add !tmp2 a b
    addl -8(%rbp), %eax
    movl %eax, -24(%rbp)
    movl -24(%rbp), %eax          ; copy c !tmp2
    movl %eax, -12(%rbp)
    movl -12(%rbp), %eax          ; copy !retval c
    movl %eax, -28(%rbp)
    movl -28(%rbp), %eax          ; épilogue : charger !retval dans %eax
    movq %rbp, %rsp
    popq %rbp
    ret
```

## 4. Les conventions à connaître

### Convention de retour des visiteurs d'expressions
Chaque `visitExpr_*` retourne un **`std::string`** : le nom de la variable ou du temporaire contenant le résultat.

```cpp
// visitExpr_const crée un temp et retourne son nom
antlrcpp::Any IRVisitor::visitExpr_const(...) {
    std::string tmp = currentCFG->newTemp();   // "!tmp0", "!tmp1", ...
    bloc->addInstruction(new IRInstrConst(bloc, tmp, val));
    return tmp;  // ← retourne le nom du temp
}

// visitExpr_id retourne directement le nom de la variable
antlrcpp::Any IRVisitor::visitExpr_id(...) {
    return ctx->ID()->getText();  // ← "a", "b", "c"
}
```

Pour **récupérer** la string retournée par un sous-visiteur :
```cpp
std::string result = std::any_cast<std::string>(visit(ctx->rhs()));
```

### Temporaires
- Créés par `currentCFG->newTemp()` → noms `!tmp0`, `!tmp1`, `!tmp2`...
- Automatiquement ajoutés dans la SymbolTable avec un offset
- Le `!` au début permet de les distinguer des vraies variables

### La variable spéciale `!retval`
- Réservée dans `buildIr()` au début
- Le `return` copie son résultat dedans : `copy !retval tmp`
- L'épilogue du BackEnd émet `movl offset_retval(%rbp), %eax` pour le charger dans le registre de retour

## 5. Comment ajouter une nouvelle instruction IR

**Exemple : ajouter `IRInstrSub` (soustraction)**

### Étape A — Déclarer dans `IR.h`
```cpp
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
```

### Étape B — Implémenter dans `IR.cpp`
```cpp
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
    // TODO
}
```

### Étape C — Brancher dans `IRVisitor.cpp`
```cpp
antlrcpp::Any IRVisitor::visitExpr_plusmoins(ifccParser::Expr_plusmoinsContext* ctx) {
    std::string lhs = std::any_cast<std::string>(visit(ctx->rhs(0)));
    std::string rhs = std::any_cast<std::string>(visit(ctx->rhs(1)));
    std::string tmp = currentCFG->newTemp();
    auto* bloc = currentCFG->getCurrentBasicBloc();

    std::string op = ctx->children[1]->getText();
    if (op == "+") {
        bloc->addInstruction(new IRInstrAdd(bloc, tmp, lhs, rhs));
    } else {
        bloc->addInstruction(new IRInstrSub(bloc, tmp, lhs, rhs));  // ← ICI
    }
    return tmp;
}
```

C'est tout. `make` et ça marche.

## 6. Instructions restantes à implémenter

| Instruction IR | Fichiers à toucher | Visiteur à brancher |
|---|---|---|
| `IRInstrSub` | IR.h, IR.cpp | `visitExpr_plusmoins` (branche `-`) |
| `IRInstrMul` | IR.h, IR.cpp | `visitExpr_multdiv` (branche `*`) |
| `IRInstrDiv` | IR.h, IR.cpp | `visitExpr_multdiv` (branche `/`) |
| `IRInstrMod` | IR.h, IR.cpp | `visitExpr_multdiv` (branche `%`) |
| `IRInstrNeg` | IR.h, IR.cpp | `visitExpr_moinsunaire` |
| `IRInstrCmpEq/Ne/Lt/Le/Gt/Ge` | IR.h, IR.cpp | `visitExpr_comparison` + `visitExpr_equality` |
| `IRInstrAnd` | IR.h, IR.cpp | `visitExpr_and` |
| `IRInstrOr` | IR.h, IR.cpp | `visitExpr_or` |
| `IRInstrXor` | IR.h, IR.cpp | `visitExpr_xor` |

**Le pattern est toujours le même** : déclarer la classe (IR.h) → implémenter constructeur + printDebug + genX86 (IR.cpp) → brancher dans le bon visiteur (IRVisitor.cpp).

## 7. Accès mémoire dans genX86

Pour accéder à l'offset d'une variable depuis une instruction IR :
```cpp
int offset = parentBloc->getCFG()->getSymbolTable()->getOffset(nomVariable);
```

Ensuite on utilise l'adressage `offset(%rbp)` en AT&T syntax.
