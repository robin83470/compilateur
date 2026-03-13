grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* '}' ;

stmt : return_stmt | declaration_stmt | assign_stmt | while_stmt | if_stmt ;

return_stmt : RETURN rhs ';' ;

declarator : ID (EQUAL rhs)? ;
declaration_stmt : 'int' declarator (',' declarator)* ';' ;
assign_stmt : ID EQUAL rhs ';' ;
while_stmt : WHILE '(' rhs ')' '{' stmt* '}' ;
WHILE : 'while' ; 

block : '{' stmt* '}' ;

if_stmt: 'if' '(' rhs ')' block ('else' 'if' '(' rhs ')' block)* ('else' block)? #if_elsifelse ;

rhs : 
    ('+'|'-'|'!') rhs                      # Expr_moinsunaire
    | rhs ('*'|'/'|'%') rhs            # Expr_multdiv
    | rhs ('+'|'-') rhs                # Expr_plusmoins
    | rhs (CMPLE|CMPGE|CMPLT|CMPGT) rhs # Expr_comparison
    | rhs (CMPEQ|CMPNE) rhs            # Expr_equality
    | rhs BAND rhs                     # Expr_and
    | rhs BOR rhs                      # Expr_or
    | rhs BXOR rhs                     # Expr_xor
    | '(' rhs ')'                      # Expr_parenthese
    | CONST                            # Expr_const
    | CHARCONST                        # Expr_char
    | ID                               # Expr_id
;

RETURN : 'return' ;
CONST : [0-9]+ ;
CHARCONST : '\'' ( '\\' . | ~('\\'|'\'') ) '\'' ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ID : [a-zA-Z_][a-zA-Z0-9_]* ;
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
BAND : '&' ;
BXOR : '^' ;
BOR  : '|' ;
BNOT : '!';

