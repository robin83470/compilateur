# PLD Compilateur
Ce projet est un compilateur pour le langage C.

je mange des crêpes
## Compilation du compilateur

Pour compiler le compilateur `ifcc`, placez-vous dans le dossier `compiler` et exécutez la commande `make` :

```bash
cd compiler
make
```

Cela génère l'exécutable `ifcc` dans le dossier `compiler`.

## Utilisation du compilateur

Le compilateur prend en entrée un fichier source C et génère du code assembleur x86-64 sur la sortie standard.

### Syntaxe

```bash
./ifcc chemin/vers/fichier.c > output.s
```

### Exemple

Pour compiler un programme simple :

```c
// exemple.c
int main() {
    return 42;
}
```

```bash
./ifcc exemple.c > exemple.s
gcc exemple.s -o exemple
./exemple
echo $?
# Devrait afficher 42
```

## Exécution des tests

Le script `ifcc-test.py` permet de lancer automatiquement les tests sur tous les programmes de test.

### Lancement des tests

```bash
python3 ifcc-test.py testfiles/
```

Ce script :
- Compile chaque programme de test avec `ifcc` et avec `gcc`
- Assemble et lie les programmes
- Exécute les programmes et compare les codes de retour
- Génère des logs et des fichiers de sortie dans le dossier `ifcc-test-output`

### Structure des tests

- `testfiles/ValidPrograms/` : Programmes C valides
- `testfiles/InvalidPrograms/` : Programmes C invalides (pour tester les erreurs)

## Fonctionnalités supportées


### Types de base
- `int` : Type entier

### Déclarations
- Déclaration de variables : `int a;`
- Déclaration avec initialisation : `int a = 5;`
- Déclarations multiples : `int a, b = 3;`

### Expressions arithmétiques
- Constantes entières : `42`
- Variables : `a`
- Opérateurs binaires :
  - Addition : `+`
  - Soustraction : `-`
  - Multiplication : `*`
  - Division : `/`
  - Modulo : `%`
- Opérateur unaire : `-` (négation)
- Parentheses : `(expr)`
- Priorités des opérateurs (selon la norme C) :
  - `*`, `/`, `%` avant `+`, `-`
  - Associativité à gauche par défaut
- Expressions imbriquées

### Instructions
- Return : `return expr;`

### Gestion des erreurs
- Erreurs de syntaxe
- Variables non déclarées
- Variables déclarées plusieurs fois
- Programmes invalides (texte brut, point-virgule manquant, accolade fermante manquante)

### Limitations
- Pas de types autres que `int`
- Pas de fonctions autres que `main`
- Pas de structures de contrôle (`if`, `while`, etc.)
- Pas de tableaux
- Pas de pointeurs
- Pas d'expressions logiques ou de comparaisons
- Pas de caractéres spéciaux dans les commentaires


## Dépendances

- ANTLR4 (runtime C++)
- GCC (pour l'assemblage et le linkage)
- Python 3 (pour les tests)
