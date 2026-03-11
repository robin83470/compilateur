grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* '}' ;

stmt : return_stmt | declaration_stmt | assign_stmt ;

return_stmt : RETURN rhs ';' ;

declarator : ID (EQUAL rhs)? ;
declaration_stmt : 'int' declarator (',' declarator)* ';' ;
assign_stmt : ID EQUAL rhs ';' ;


rhs : 
    ('+'|'-') rhs                      # Expr_moinsunaire
    | rhs ('*'|'/'|'%') rhs            # Expr_multdiv
    | rhs ('+'|'-') rhs                # Expr_plusmoins
    | rhs (CMPLE|CMPGE|CMPLT|CMPGT) rhs # Expr_comparison
    | rhs (CMPEQ|CMPNE) rhs            # Expr_equality
    | rhs BAND rhs                     # Expr_and
    | rhs BXOR rhs                     # Expr_xor
    | rhs BOR rhs                      # Expr_or
    | '(' rhs ')'                      # Expr_parenthese
    | CONST                            # Expr_const
    | ID                               # Expr_id


RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ID : [a-zA-Z_][a-zA-Z0-9_]* ;
EQUAL : '=' ;

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

