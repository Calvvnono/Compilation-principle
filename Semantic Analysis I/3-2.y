%{
#include <stdio.h>
%}

%union {
    int ival;
}

%token <ival> NUM
%nterm <ival> exp

%left '-'  // 更高优先级
%left '+'  // 更低优先级

%%

exp:
    exp '+' exp  { $$ = $1 + $3; }
    | exp '-' exp { $$ = $1 - $3; }
    | NUM        { $$ = $1; }
    ;

%%

int main() {
    yyparse();
    return 0;
}

int yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
    return 0;
}