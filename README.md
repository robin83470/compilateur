# PLD Compilateur

README refait depuis zero avec etat reel du projet base sur execution complete des tests.

Date du bilan : 2026-03-31

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
./ifcc [--target x86_64|arm64] path/to/file.c > output.s
gcc output.s -o output
./output
echo $?
```

## 3. Execution des tests (fait dans ce bilan)

Commande executee pour ce README :

```bash
python3 ifcc-test.py testfiles
```

Resultat detaille ecrit dans :

- test_results.csv

## 4. Bilan global des tests

Resultats globaux (tous tests, implementes et non implementes) :

- Total : 449
- OK : 403
- FAIL : 46

Par groupe :

- Valid : 140 total, 138 OK, 2 FAIL
- Invalid : 130 total, 123 OK, 7 FAIL
- NotImplementedYet : 179 total, 142 OK, 37 FAIL

Types d echecs observes :

- 15 cas : compilateur rejette un programme valide
- 14 cas : resultat d execution different de GCC
- 13 cas : assembleur genere incorrect
- 4 cas : compilateur accepte un programme invalide

## 5. Ce qui est cense marcher (et marche majoritairement)

Suites stables (quasi toutes vertes) :

- Base : retours, declarations, affectations simples
- Arithmetique : priorites/associativite/unaires
- While, structures conditionnelles `if`, `if/else`, `else if`, imbrication de conditions, commentaires
- Appels I/O de base : getchar et putchar avec argument (`putchar(expr);`)
- Affectation composee dans ValidPrograms
- Une grande partie de switch, pointeurs, comparaisons binaires

Sur les suites implementees uniquement (hors NotImplementedYet) :

- 270 tests executes
- 261 OK
- 9 FAIL

## 6. Ce qui ne marche pas (liste exhaustive des FAIL)

### 6.1 Echecs hors NotImplementedYet (priorite correction)

- testfiles-InvalidPrograms-ComparaisonBinaire-or_after_if
- testfiles-InvalidPrograms-Functions-func_no_type
- testfiles-InvalidPrograms-Functions-func_too_few_args
- testfiles-InvalidPrograms-Functions-func_too_many_args
- testfiles-InvalidPrograms-Functions-func_undefined
- testfiles-InvalidPrograms-block-block_no_block_keyword
- testfiles-InvalidPrograms-block-block_wrong_return_type
- testfiles-ValidPrograms-ComparaisonBinaire-logical_and_or_precedence
- testfiles-ValidPrograms-Pointeurs-double_deref_read

### 6.2 Echecs NotImplementedYet (fonctionnalites en cours)

- testfiles-NotImplementedYet-InvalidProgram-OperateursParesseux-18_if_without_block_with_logic
- testfiles-NotImplementedYet-InvalidProgram-switch-11_switch_no_body
- testfiles-NotImplementedYet-ValidPrograms-Comparaison-priorite
- testfiles-NotImplementedYet-ValidPrograms-Functions-func_ext_keyword
- testfiles-NotImplementedYet-ValidPrograms-Functions-func_invalid_return_type
- testfiles-NotImplementedYet-ValidPrograms-Functions-func_return_missing
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_call_in_expr
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_default
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_expr_arg
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_multi
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_multi_param_expr
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_nested_call
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_no_param
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_order
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_param_chain
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_params_add
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_return_var
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_scope
- testfiles-NotImplementedYet-ValidPrograms-Functions-function_simple_return
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-10_logic_with_blocks
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-12_or_chain_with_zero_guard
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-13_lazy_in_while_guard
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-14_lazy_if_else_complex
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-16_nested_parentheses_complex
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-17_with_unary_and_compare
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-2_or_guard_division
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-3_and_or_nested
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-6_while_lazy_condition
- testfiles-NotImplementedYet-ValidPrograms-OperateursParesseux-7_chain_and_or
- testfiles-NotImplementedYet-ValidPrograms-alphabet
- testfiles-NotImplementedYet-ValidPrograms-bitwise_or
- testfiles-NotImplementedYet-ValidPrograms-block-block_var_scope
- testfiles-NotImplementedYet-ValidPrograms-print_int
- testfiles-NotImplementedYet-ValidPrograms-switch-10_switch_negative_case
- testfiles-NotImplementedYet-ValidPrograms-switch-14_switch_local_var_in_case
- testfiles-NotImplementedYet-ValidPrograms-switch-15_switch_putchar
- testfiles-NotImplementedYet-ValidPrograms-switch-7_switch_default_middle
- testfiles-NotImplementedYet-bitwise_or

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

Important : une partie des erreurs semantiques est encore geree via sortie stderr + exit direct dans la table des symboles, pas via une infrastructure d erreur unifiee.

## 8. Cas mentionnes en revue : attendu vs observe

Verification faite pendant la redaction du README :

- return 5 ++ 5;
  - Attendu : erreur.
  - Observe : rejete (erreur de parsing). Donc ce point semble corrige.

- y = x - - x;
  - Attendu : valide (equivalent a x - (-x)).
  - Observe : compile et execute, code retour observe 6 pour x=3.

- y = !0;
  - Attendu (selon remarque): erreur de syntaxe.
  - Observe : compile et execute, code retour 1.
  - Note : avec la grammaire actuelle, ! est explicitement supporte, donc ce comportement est coherent avec l implementation courante.

- int c = 'A';
  - Attendu : equivalent a 65.
  - Observe : compile et execute, code retour 65.

- Verification lexer.getNumberOfSyntaxErrors dans main
  - Observe : present et teste dans main (en plus de parser.getNumberOfSyntaxErrors).

- Programme sans return
  - Attendu projet : interdit (doit etre rejete).
  - Observe : accepte actuellement, et peut produire un executable avec retour non contractuel (exemple observe a 0).

Conclusion sur ce point : la politique projet doit etre explicitee et enforcee semantiquement (au minimum, rejet des fonctions int sans return garanti).

## 9. Ce qui est officiellement contractuel vs en cours

Contractuel aujourd hui :

- Ce qui est dans ValidPrograms et InvalidPrograms, sous reserve des 9 regressions listees en section 6.1.

En cours (non contractuel) :

- Contenu de NotImplementedYet, notamment fonctions avancees, operateurs paresseux complexes, certains cas switch, print_int, et variantes supplementaires.

Note sur les conditions :

- Les formes `if (...) stmt;`, `if (...) { ... }`, `if (...) ... else ...` et `else if (...) ...` sont prises en charge.
- Les conditions imbriquees et l'utilisation de conditions a l'interieur de blocs ou de boucles sont egalement couvertes par les tests valides actuels.

## 10. Pistes de correction prioritaires

- Corriger les 9 FAIL hors NotImplementedYet en premier.
- Rendre obligatoire un return valide pour les fonctions int.
- Unifier le reporting d erreurs semantiques (exceptions + contexte source) au lieu de sorties abruptes.
- Finaliser l implementation IR/backend des appels de fonctions utilisateur (source principale de FAIL en NotImplementedYet).
