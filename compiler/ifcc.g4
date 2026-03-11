grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' stmt* '}' ;

stmt : return_stmt | declaration_stmt | assign_stmt ;

return_stmt : RETURN rhs ';' ;

declarator : ID (EQUAL rhs)? ;
declaration_stmt : 'int' declarator (',' declarator)* ';' ;
assign_stmt : ID EQUAL rhs ';' ;


rhs : 
    ('+'|'-') rhs            # Expr_moinsunaire
    | rhs ('*'|'/'|'%') rhs   # Expr_multdiv
    | rhs ('+'|'-') rhs   # Expr_plusmoins
    | '(' rhs ')'          # Expr_parenthese
    | CONST                 # Expr_const
    | ID                    # Expr_id
    ;


RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);
ID : [a-zA-Z_][a-zA-Z0-9_]* ;
EQUAL : '=' ;

