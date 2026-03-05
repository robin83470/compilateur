grammar ifcc;

axiom : prog EOF ;

prog : 'int' 'main' '(' ')' '{' expr* return_stmt '}' ;



expr
  : assignment ';'
  ;

assignment
  : 'int' v1=VAR '=' c1=CONST  # assignment_decla_const
  | 'int' v1=VAR '=' v2=VAR   # assignment_decla_var
  | v1=VAR '=' v2=VAR   # assignment_vv
  | v1=VAR '=' c1=CONST # assignment_vc
  ;

return_stmt
: RETURN CONST ';' # return_const_stmt
| RETURN VAR ';' # return_var_stmt ;

RETURN : 'return' ;
CONST : [0-9]+ ;
COMMENT : '/*' .*? '*/' -> skip ;
DIRECTIVE : '#' .*? '\n' -> skip ;
WS    : [ \t\r\n] -> channel(HIDDEN);


VAR : 'a';
