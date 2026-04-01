# PLD Compilateur

README mis a jour avec le dernier run complet des tests.

Date du bilan : 2026-04-01

## 1. Ce qui change vraiment par rapport a un compilateur C classique

Notre compilateur est volontairement un sous-ensemble de C, oriente TP, avec des choix differents d un compilateur industriel :

- Focus sur un sous-ensemble du langage (grammaire ANTLR + visiteurs IR).
- Sortie directe en assembleur cible x86_64 ou arm64, pas de phases d optimisation avancees.
- Verification semantique minimaliste mais explicite (types principalement int/int*).
- Suite de tests structuree en trois blocs :
  - ValidPrograms : comportement attendu stable.
  - InvalidPrograms : programmes a rejeter.
  - NotImplementedYet : fonctionnalites en cours, pas contractuelles.

## 2. Compilation et utilisation

Compiler le projet :

```bash
cd compiler
make
```

Utiliser le compilateur :

```bash
./ifcc [--target x86_64|arm64] path/to/file.c 

```

## 3. Execution des tests (fait dans ce bilan)

Commande executee pour ce README :

python3 ifcc-test.py testfiles/

Resultat detaille ecrit dans :

- test_results.csv

## 4. Bilan global des tests

Resultats globaux (tous tests, implementes et non implementes) :

- Total : 451
- OK : 440
- FAIL : 11

Par groupe :

- Valid : 280 total, 280 OK, 0 FAIL
- Invalid : 160 total, 160 OK, 0 FAIL
- NotImplementedYet : 11 total, 0 OK, 11 FAIL

Interpretation :

- Les suites implementees sont tres majoritairement stables.
- Les echecs en NotImplementedYet sont attendus tant que la fonctionnalite n est pas finalisee.
- Certains tests passent chez un memlbre de l'hexanome mais pas chez tout le monde (exemple: block_wrong_return_type)

## 5. Ce qui est cense marcher (et marche)

Sur les suites considerees implementees (ValidPrograms + InvalidPrograms), le run courant montre un comportement stable.

### 5.1 Noyau du langage

- Programmes avec fonction principale de type int.
- Declarations simples et multiples de variables entieres.
- Initialisation, affectation et reutilisation de variables.
- Return avec expression arithmetique ou variable.

Exemples de cas verifies dans les tests :

- return constant et return variable.
- declarations successives puis calcul.
- affectation puis retour.

### 5.2 Expressions et priorites

- Operateurs arithmetiques binaires : +, -, *, /, %.
- Operateurs unaires utilises dans les tests : -, !.
- Parentheses et expressions imbriquees.
- Priorites et associativite gauche conformes sur la majorite des cas de test valides.

Exemples de comportements verifies :

- combinaison addition/multiplication avec priorite.
- modulo dans des expressions composees.
- unaire moins sur constante et variable.

### 5.3 Structures de controle

- if simple.
- if/else.
- chaines else if.
- while avec conditions arithmetiques et comparaisons.
- break et continue dans while.

Exemples verifies :

- conditions imbriquees dans des blocs.
- while avec mise a jour de plusieurs variables.
- combinaison while + if.

### 5.4 Comparaisons et bitwise couverts

- Comparaisons : <, <=, >, >=, ==, !=.
- Bitwise : &, |, ^ sur les cas valides de la suite principale.
- Cas complexes avec parenthesage dans ComparaisonBinaire.

### 5.5 I/O et caractere

- getchar() sur les cas de lecture prevus.
- putchar(expr) sur les formes actuellement gerees par le compilateur.
- Constantes caracteres interpretees correctement dans les cas simples (ex: 'A' -> 65 observe).

### 5.6 Robustesse de rejet (InvalidPrograms)

Les programmes invalides de la suite principale sont majoritairement rejetes correctement :

- operandes manquantes,
- parenthese/accolade manquante,
- tokens illegaux,
- erreurs de declaration ou d usage de symboles.

### 5.7 Resume quantitatif sur ce qui marche

- ValidPrograms : 280 sur 280 OK.
- InvalidPrograms : 160 sur 160 OK.
- Donc 440 tests passes sur les zones considerees implementees.




## 5bis. Limitations importantes sur les fonctions

Les points suivants ne sont pas implementes completement dans les zones en cours :

### Signatures de fonction
- Parametres de type pointeur : int foo(int* p) non supporte
  - Les parametres sont principalement traites comme int brut
  - Impossible de passer des pointeurs en paramètres

- Types de retour autres que int :
  - Pas de support stable pour void, char, ou autres types
  - Les fonctions sont considerees comme retournant int

- Return sans valeur : return; non supporte de maniere generale
  - Chaque fonction utilisateur est attendue avec return expression



## 6. Ce qui ne marche pas (NotImplementedYet)


Ces echecs sont attendus et ne sont pas des regressions bloquantes a ce stade :

- 11_switch_no_body
- 14_switch_local_var_in_case
- bitwise_or
- block_var_scope
- func_ext_keyword
- func_invalid_return_type
- func_no_type
- func_return_missing
- or_after_if
- priorite

## 7. Messages d erreurs et warnings (actuels)

Messages observes/emis par le compilateur :

- error: syntax error during parsing
- Erreur lexicale
- Erreur : le symbole '<nom>' est deja declare.
- Erreur : le symbole '<nom>' n'existe pas.
- Erreur : type inconnu '<type>'.
- Erreur de type (...)
- Avertissement : la variable '<nom>' a ete declaree mais n est pas utilisee.
- error: unsupported target '<valeur>' (expected x86_64 or arm64)
- error: unknown option '<option>'
- error: cannot read file: <path>






## 8. Ce qui est officiellement contractuel vs en cours

Contractuel aujourd hui :

- Ce qui est dans ValidPrograms et InvalidPrograms.

En cours (non contractuel) :

- Contenu de NotImplementedYet.
- Si un test NotImplementedYet echoue, c est considere normal tant que la fonctionnalite n est pas finalisee.

Note sur les conditions :

- Les formes `if (...) stmt;`, `if (...) { ... }`, `if (...) ... else ...` et `else if (...) ...` sont prises en charge.
- Les conditions imbriquees et l'utilisation de conditions a l'interieur de blocs ou de boucles sont egalement couvertes par les tests valides actuels.

## 10. Pistes de correction prioritaires

- Corriger block_wrong_return_type.
- Finaliser progressivement les cas NotImplementedYet (switch/fonctions/bitwise/logique) puis migrer les tests vers ValidPrograms ou InvalidPrograms.
- Renforcer la regle semantique imposant un return pour les fonctions int si c est la politique du projet.
- Unifier le reporting d erreurs semantiques (exceptions + contexte source) au lieu de sorties abruptes.
- Rajouter des fonctionnalités
