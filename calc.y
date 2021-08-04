%{
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <string.h>
#include <sys/random.h>
#include "sevaluator.h"

extern int yylex();
extern int yyparse();

void yyerror(const char *s);

static FullResult full_result;
static Mode mode;
static HistoryList *history_list;
static gmp_randstate_t random_state;

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
calc: expr { mpq_init(full_result.result.v_rat); mpq_set(full_result.result.v_rat, $1); full_result.result_type = R_RAT; }
expr: expr PLUS fact { mpq_init($$); mpq_add($$, $1, $3); }
    | expr MINUS fact { mpq_init($$); mpq_sub($$, $1, $3); }
    | fact { mpq_init($$); mpq_set($$, $1); }
    ;
fact: fact MULTIPLY number { mpq_init($$); mpq_mul($$, $1, $3); }
    | fact DIVIDE number { mpq_init($$); mpq_div($$, $1, $3); }
    | number { mpq_init($$); mpq_set($$, $1); }
    | MINUS fact { mpq_init($$); mpq_neg($$, $2); }
    | PLUS fact { mpq_init($$); mpq_set($$, $2); }
    ;
number: literal { mpq_init($$); mpq_set($$, $1); }
    | LEFT expr RIGHT { mpq_init($$); mpq_set($$, $2); }
    ;
literal: 
    FUNC_HIST F_LEFT INTEGER F_RIGHT { 
        if (mode == NO_HISTORY) { YYABORT; } 
        if (mpz_cmp_ui($3, 1000000) > 0) {
            YYABORT;
        }
        int index = (int) mpz_get_ui($3);
        const char *hist_result = sevaluator_history_get(history_list, index);
        
        if (!hist_result) {
            YYABORT;
        }
        mpq_init($$);
        mpq_set_str($$, hist_result, 10);
    }
    | FUNC_RANDOM F_LEFT F_RIGHT {

        unsigned long temp = gmp_urandomb_ui(random_state, 8);

        mpq_init($$);
        mpq_set_ui($$, temp, 1);
    }
    | RATIONAL { mpq_init($$); mpq_set($$, $1); }
    | INTEGER { mpq_init($$); mpq_set_z($$, $1); }
    ;

%%

/* int main() {
    do {
        yyparse();
    } while(!feof(stdin));
    return 0;
} */

void init_random_state() {
    gmp_randinit_default(random_state);
    unsigned long seed;
    getrandom(&seed, sizeof(seed), GRND_RANDOM);
    gmp_randseed_ui(random_state, seed);
}

int sevaluator_calc_no_history(const char *input, char **output) {
    init_random_state();
    mode = NO_HISTORY;
    yy_scan_string(input);
    int error = yyparse();
    if (error) {
        yylex_destroy();
        gmp_randclear(random_state);
        return 1;
    }
    if (full_result.result_type == R_RAT) {
        *output = mpq_get_str(NULL, 10, full_result.result.v_rat);
        mpq_clear(full_result.result.v_rat);
    } else {
        yylex_destroy();
        gmp_randclear(random_state);
        return 1;
    }
    yylex_destroy();
    gmp_randclear(random_state);
    return 0;
}

int sevaluator_calc(const char *input, char **output, HistoryList *list) {
    init_random_state();
    mode = DEFAULT;
    history_list = list;
    yy_scan_string(input);
    int error = yyparse();
    if (error) {
        history_list = NULL;
        yylex_destroy();
        gmp_randclear(random_state);
        return 1;
    }
    if (full_result.result_type == R_RAT) {
        *output = mpq_get_str(NULL, 10, full_result.result.v_rat);
        mpq_clear(full_result.result.v_rat);
        sevaluator_history_push(history_list, *output);
    } else {
        history_list = NULL;
        yylex_destroy();
        gmp_randclear(random_state);
        return 1;
    }
    history_list = NULL;
    yylex_destroy();
    gmp_randclear(random_state);
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}