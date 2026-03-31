grammar ifcc;

axiom : prog EOF ;

prog : function* ;

function : 'int' ID '(' paramList? ')' '{' stmt* '}'
         | 'int' 'main' '(' ')' '{' stmt* '}' ;

paramList : 'int' ID (',' 'int' ID)* ;

stmt : return_stmt | declaration_stmt | assign_stmt | while_stmt | if_stmt | switch_stmt | break_stmt | continue_stmt | empty_stmt | block ;

empty_stmt : ';' ;
expr_stmt : rhs ';' ;
return_stmt : RETURN rhs ';' ;

declarator : pointer_prefix? ID (EQUAL rhs)? ;
pointer_prefix : '*' pointer_prefix? ;
declaration_stmt : 'int' declarator (',' declarator)* ';' ;
assign_stmt : lvalue assign_op rhs ';' ;
while_stmt : WHILE '(' rhs ')' block ;
break_stmt : BREAK ';' ;
continue_stmt : CONTINUE ';' ;
switch_stmt : SWITCH '(' rhs ')' '{' switch_case* switch_default? '}' ;
switch_case : CASE switch_value ':' stmt* ;
switch_default : DEFAULT ':' stmt* ;
switch_value : CONST | CHARCONST ;

assign_op : EQUAL | PLUSEQ | MINUSEQ | MULEQ | DIVEQ | MODEQ ;
WHILE : 'while' ;
BREAK : 'break' ;
CONTINUE : 'continue' ;
SWITCH : 'switch' ;
CASE : 'case' ;
DEFAULT : 'default' ;

block : '{' stmt* '}' ;

// Allow 'else if' by using parser-level sequencing; no special token needed.
if_stmt: 'if' '(' rhs ')' stmt ('else' stmt)? ;

lvalue
    : ID # Lvalue_id
    | '*' lvalue # Lvalue_deref
    | '(' lvalue ')' # Lvalue_parenthese
;

rhs :
    ('+'|'-'|BNOT) rhs # Expr_moinsunaire
    | '*' rhs # Expr_deref
    | BAND lvalue # Expr_addrof
    | rhs ('*'|'/'|'%') rhs # Expr_multdiv
    | rhs ('+'|'-') rhs # Expr_plusmoins
    | rhs (CMPLE|CMPGE|CMPLT|CMPGT) rhs # Expr_comparison
    | rhs (CMPEQ|CMPNE) rhs            # Expr_equality
    | rhs LAND rhs                     # Expr_land
    | rhs LOR rhs                      # Expr_lor
    | rhs BAND rhs                     # Expr_and
    | rhs BOR rhs                      # Expr_or
    | rhs BXOR rhs                     # Expr_xor
    | '(' rhs ')'                      # Expr_parenthese
    | GETCHAR '(' ')'                  # Expr_getchar
    | PUTCHAR '(' io_arg ')'           # Expr_putchar
    | ID '(' rhsList? ')' # Expr_funcCall
    | CONST                            # Expr_const
    | CHARCONST                        # Expr_char
    | ID                               # Expr_id
;

io_arg : GETCHAR '(' ')' | CONST | CHARCONST | ID ;

rhsList : rhs (',' rhs)* ;
RETURN : 'return' ;
GETCHAR : 'getchar' ;
PUTCHAR : 'putchar' ;
CONST : [0-9]+ ;
CHARCONST : '\'' ( '\\' . | ~('\\'|'\'') ) '\'' ;
LINE_COMMENT : '//' ~[\r\n]* -> skip ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ID : [a-zA-Z_][a-zA-Z0-9_]* ;
PLUSEQ : '+=' ;
MINUSEQ : '-=' ;
MULEQ : '*=' ;
DIVEQ : '/=' ;
MODEQ : '%=' ;
EQUAL : '=' ;
INCR  : '++' ;
DECR  : '--' ;

// Tokens de comparaison (ordre important : <= et >= AVANT < et >)
CMPLE : '<=' ;
CMPGE : '>=' ;
CMPLT : '<' ;
CMPGT : '>' ;
CMPEQ : '==' ;
CMPNE : '!=' ;
LAND : '&&' ;
LOR : '||' ;
BAND : '&' ;
BXOR : '^' ;
BOR  : '|' ;
BNOT : '!';
