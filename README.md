# PLD Compilateur

README technique du projet : etat des fonctionnalites, couverture de tests, limites connues.


## 1. Positionnement du compilateur

Ce compilateur est un sous-ensemble de C, developpe pour un projet pedagogique.

Différences majeures avec un compilateur C complet :

- couverture volontairement limitee a un noyau de langage ;
- sorties assembleur ciblees (x86_64 / arm64) sans pipeline d optimisation avance ;
- validation tres orientee tests fonctionnels du projet.

## 2. Build et execution

Compilation du compilateur :

```bash
cd compiler
make
```

Utilisation :

```bash
./ifcc [--target x86_64|arm64] path/to/file.c > output.s
gcc output.s -o output
./output
```

## 3. Campagne de tests utilisee pour ce README

Commande executee :

```bash
python3 ifcc-test.py testfiles/
```

Fichier de synthese :

- test_results.csv

## 4. Bilan global des tests

- Total : 452
- OK : 440
- FAIL : 12

Repartition :

- Valid : 280/280 OK
- Invalid : 160/160 OK
- NotImplementedYet : 0/12 OK

Interpretation :

- Les fonctionnalites considereees implementees (Valid/Invalid) sont stables.
- Les 12 FAIL sont tous situes dans NotImplementedYet, donc attendus a ce stade.

## 5. Couverture de tests actuelle

Volumes de tests C detectes :

- testfiles/ValidPrograms : 281 fichiers
- testfiles/InvalidPrograms : 170 fichiers
- testfiles/NotImplementedYet : 13 fichiers

Domaines principaux couverts par tests actifs :

- Arithmetic
- Unary
- Comparaison / ComparaisonBinaire
- if / while / switch / block
- Pointeurs
- FunctionCalls
- AffectationComposee
- OperateursParesseux
- MultipleReturn
- commentaire

## 6. Ce qui marche (suites actives)

Le run courant valide les points suivants dans les suites actives (ValidPrograms + InvalidPrograms) :

- declarations, affectations, retours ;
- expressions arithmetiques et unaires ;
- priorites et parenthesage sur les cas couverts ;
- structures de controle if / else if / while / switch (cas actifs) ;
- break / continue en boucle ;
- comparaisons et bitwise sur les jeux de tests actifs ;
- getchar / putchar sur les variantes testees ;
- robustesse de rejet des programmes invalides (160/160).

## 7. Ce qui ne marche pas encore (NotImplementedYet)

Les echecs ci-dessous sont classes NotImplementedYet et donc non bloquants a ce stade :

- 11_switch_no_body
- 14_switch_local_var_in_case
- bitwise_or
- block_var_scope
- block_wrong_return_type
- func_ext_keyword
- func_invalid_return_type
- func_no_type
- func_return_missing
- or_after_if
- print_int
- priorite
- affecattions multiples (ex: a = b = 4;)
- arithmétiques des pointeurs
- Déréférencement récursif sur les pointeurs
- Comparaison entre pointeurs non géré
- include
- les autres types que int/pointeurs de 'int'
- les fonctions sans valeur de retour
- structures de contrôle 'for', 'do ... while'




## 8. Limites actuelles du compilateur

- Couverture partielle du langage C (subset projet).
- Plusieurs cas de fonctions utilisateur restent dans NotImplementedYet.
- Signatures de fonctions non triviales encore incomplètes selon les cas.
- Heterogeneite des diagnostics entre erreurs parser et erreurs semantiques.



