# PLD Compilateur
Ce projet est un compilateur pour le langage C.

## Compilation du compilateur

Pour compiler le compilateur `ifcc`, placez-vous dans le dossier `compiler` et exécutez la commande `make` :

```bash
cd compiler
make
```

Cela génère l'exécutable `ifcc` dans le dossier `compiler`.

## Utilisation du compilateur

Le compilateur prend en entrée un fichier source C et génère du code assembleur sur la sortie standard.

### Cibles supportées

- `x86_64` : Intel 64 bits
- `arm64` : Apple Silicon (M series)

Par défaut, la cible est `x86_64`.

### Syntaxe

```bash
./ifcc [--target <x86_64|arm64>] chemin/vers/fichier.c > output.s
```

### Exemples de génération

```bash
# Intel
./ifcc --target x86_64 exemple.c > exemple-x86.s

# Apple Silicon Mac (M1/M2/M3/...)
./ifcc --target arm64 exemple.c > exemple-arm64.s
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
./ifcc --target x86_64 exemple.c > exemple.s
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

Par défaut, si `--target` n'est pas fourni, la suite de tests utilise `x86_64`.

Vous pouvez préciser la cible testée :

```bash
# Sur machine Intel
python3 ifcc-test.py --target x86_64 testfiles/

# Sur machine Apple Silicon
python3 ifcc-test.py --target arm64 testfiles/
```

Ce script :
- Compile chaque programme de test avec `ifcc --target ...` et avec `gcc`
- Assemble et lie les programmes
- Exécute les programmes et compare les codes de retour
- Génère des logs et des fichiers de sortie dans le dossier `ifcc-test-output`

### Structure des tests

- `testfiles/ValidPrograms/` : Programmes C valides
- `testfiles/InvalidPrograms/` : Programmes C invalides (pour tester les erreurs)
- `testfiles/NotImplementedYet/` : Réservé aux cas non implémentés (actuellement vide)


## Fonctionnalités supportées

### Types de base
- `int` : Type entier

### Structure du programme
- Une fonction `int main() { ... }`

### Déclarations
- Déclaration de variables : `int a;`
- Déclaration avec initialisation : `int a = 5;`
- Déclarations multiples : `int a, b = 3;`
- Affectation : `a = expr;`

### Expressions arithmétiques
- Constantes entières : `42`
- Variables : `a`
- Opérateurs unaires : `+`, `-`, `!`
- Opérateurs binaires arithmétiques : `+`, `-`, `*`, `/`, `%`
- Opérateurs de comparaison : `<`, `<=`, `>`, `>=`, `==`, `!=`
- Opérateurs binaires bit à bit : `&`, `|`, `^`
- Parenthèses : `(expr)`
- Priorités des opérateurs (selon la norme C) :
  - `!` et les unaires avant les binaires
  - `*`, `/`, `%` avant `+`, `-`
  - comparaisons et égalités après arithmétique
  - `&`, `^`, `|` ensuite
  - Associativité à gauche par défaut
- Expressions imbriquées

### Instructions
- `return expr;`
- `if (...) { ... }`
- `if (...) { ... } else if (...) { ... } else { ... }`
- `while (...) { ... }`

### Prétraitement et commentaires
- Lignes de directives préprocesseur (`#...`) ignorées par le parser
- Commentaires C bloc `/* ... */`

### Gestion des erreurs
- Erreurs lexicales
- Erreurs de syntaxe
- Variables non déclarées
- Variables déclarées plusieurs fois
- Avertissement pour variables déclarées mais non utilisées

### Limitations
- Uniquement `int main()` sans paramètres
- Pas de types autres que `int` (pas de `char`, `float`, etc.)
- Pas d'appels de fonctions
- Pas de tableaux, pointeurs, structures
- Pas de `for`, `do ... while`, `switch`, `break`, `continue`
- Pas d'opérateurs logiques court-circuit (`&&`, `||`)
- Littéraux de caractère non pris en charge côté génération de code


## Dépendances

- ANTLR4 (runtime C++)
- GCC (pour l'assemblage et le linkage)
- Python 3 (pour les tests)
