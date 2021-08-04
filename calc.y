%{
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include "sevaluator.h"

extern int yylex();
extern int yyparse();

typedef union result {
    mpz_t v_int;
    mpq_t v_rat;
    mpf_t v_flt;
} Result;

typedef enum result_type {
    R_INT,
    R_RAT,
    R_FLT,
} ResultType;

void yyerror(const char *s);

static ResultType result_type;
static Result result;

%}

%union {
    mpz_t v_integer;
    mpq_t v_rational;
    mpf_t v_float;
}

%token<v_integer> INTEGER;
%token<v_rational> RATIONAL;
%token PLUS MINUS MULTIPLY DIVIDE POWER MOD SQRT LEFT RIGHT F_LEFT F_RIGHT COMMA FUNC_HIST FUNC_RANDOM FACTORIAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD

%type<v_rational> expr
%type<v_rational> fact
%type<v_rational> number
%type<v_rational> literal

%start calc

%destructor { mpq_clear($$); } <v_rational>
%destructor { mpz_clear($$); } <v_integer>
%destructor { mpf_clear($$); } <v_float>

/* %parse-param { ResultType *result_type } { Result *result } */

%%
calc: expr { mpq_set(result.v_rat, $1); result_type = R_RAT; }
expr: expr PLUS fact { mpq_add($$, $1, $3); }
    | expr MINUS fact { mpq_sub($$, $1, $3); }
    | fact { mpq_set($$, $1); }
    ;
fact: fact MULTIPLY number { mpq_mul($$, $1, $3); }
    | fact DIVIDE number { mpq_div($$, $1, $3); }
    | number { mpq_set($$, $1); }
    | MINUS fact { mpq_neg($$, $2); }
    | PLUS fact { mpq_set($$, $2); }
    ;
number: literal { mpq_set($$, $1); }
    | LEFT expr RIGHT { mpq_set($$, $2); }
    ;
literal: RATIONAL { mpq_set($$, $1); }
    ;

%%

/* int main() {
    do {
        yyparse();
    } while(!feof(stdin));
    return 0;
} */

int calc(const char *input, char **output) {
    yy_scan_string(input);
    int error = yyparse();
    if (error) {
        return 1;
    }
    if (result_type == R_RAT) {
        *output = mpq_get_str(NULL, 10, result.v_rat);
        mpq_clear(result.v_rat);
        return 0;
    }
    yylex_destroy();
    return 1;
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}