%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int yylex(void);
void yyerror(const char *s);
%}

%define api.value.type {double}

%token NUM
%left '+' '-'
%left '*' '/'
%right '^'
%right 'n'  // 单目负号（如：n 3 => -3）

%%

input:
    /* 空 */
    | input line
    ;

line:
    '\n'
    | exp '\n'    { printf("%.10g\n", $1); }
    ;

exp:
    NUM           { $$ = $1; }
    | exp exp '+' { $$ = $1 + $2; }
    | exp exp '-' { $$ = $1 - $2; }
    | exp exp '*' { $$ = $1 * $2; }
    | exp exp '/' { $$ = $1 / $2; }
    | exp exp '^' { $$ = pow($1, $2); }
    | exp 'n'     { $$ = -$1; }
    ;

%%

int yylex(void) {
    int c;
    while ((c = getchar()) == ' ' || c == '\t') ; // 跳过空格和制表符

    if (c == '.' || (c >= '0' && c <= '9')) {
        ungetc(c, stdin);
        scanf("%lf", &yylval);
        return NUM;
    }

    if (c == EOF || c == '!') return 0;

    return c;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    return yyparse();
}