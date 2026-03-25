grammar ifcc;

axiom : prog EOF ;

prog : function* ;

function : 'int' ID '(' paramList? ')' '{' stmt* '}'
         | 'int' 'main' '(' ')' '{' stmt* '}' ;

paramList : 'int' ID (',' 'int' ID)* ;

stmt : return_stmt | declaration_stmt | assign_stmt | while_stmt | if_stmt | break_stmt | continue_stmt | empty_stmt | block | expr_stmt ;

empty_stmt : ';' ;
expr_stmt : rhs ';' ;
return_stmt : RETURN rhs ';' ;

declarator : pointer_prefix? ID (EQUAL rhs)? ;
pointer_prefix : '*' pointer_prefix? ;
declaration_stmt : 'int' declarator (',' declarator)* ';' ;
assign_stmt : lvalue EQUAL rhs ';' ;
while_stmt : WHILE '(' rhs ')' block ;
break_stmt : BREAK ';' ;
continue_stmt : CONTINUE ';' ;


WHILE : 'while' ;
BREAK : 'break' ;
CONTINUE : 'continue' ;

block : '{' stmt* '}' ;

ELSEIF : 'else' [ \t\r\n]+ 'if' ;

if_stmt: 'if' '(' rhs ')' block (ELSEIF '(' rhs ')' block)* ('else' block)? #if_elsifelse ;

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
