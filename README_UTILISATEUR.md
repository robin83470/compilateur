# Guide Utilisateur - ifcc

Ce guide est destine a une personne qui vient de cloner le depot et veut simplement compiler et utiliser `ifcc`.

## 1. Prerequis

Il faut avoir :

- `g++` (C++17)
- `make`
- `python3`
- `gcc` (pour assembler/lier les fichiers `.s` generes)
- runtime ANTLR4 C++ (selon votre machine et votre config du projet)

## 2. Recuperer le projet

```bash
git clone <url-du-projet>
cd compilateur
```

## 3. Compiler le compilateur

```bash
cd compiler
make
```

A la fin, l executable `ifcc` est genere dans le dossier `compiler/`.

## 4. Compiler un programme C avec ifcc

Depuis le dossier `compiler/` :

```bash
./ifcc --target x86_64 ../testfiles/ValidPrograms/2_return42.c > prog.s
gcc prog.s -o prog
./prog
echo $?
```

Resultat attendu : `42`.



## 5. Lancer les tests

Depuis la racine du depot (`compilateur/`) :

```bash
python3 ifcc-test.py testfiles/
```

Les sorties detaillees sont generees dans :

- `ifcc-test-output/`
- `test_results.csv`

## 6. Interpretrer rapidement les resultats

- `ValidPrograms` : les programmes doivent compiler/s executer correctement.
- `InvalidPrograms` : les programmes doivent etre rejetes.
- `NotImplementedYet` : les echecs sont normaux tant que les fonctionnalites ne sont pas finalisees.



## 7. Portee actuelle

Le compilateur couvre un sous-ensemble de C (int, expressions, if/while, une partie de switch, etc.).

Les cas non finalises sont places dans `testfiles/NotImplementedYet/`.
