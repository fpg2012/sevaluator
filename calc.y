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

static FullResult final_result;
static Mode mode;
static HistoryList *history_list;
static gmp_randstate_t random_state;
static ErrorType error_type;

#define SE_DESTROY(a) sevaluator_result_destroy(&(a))
#define SE_COPY(a, b) sevaluator_result_copy(&(a), &(b))

#define SE_ADD(a, b, c) sevaluator_result_add(&(a), &(b), &(c))
#define SE_SUB(a, b, c) sevaluator_result_sub(&(a), &(b), &(c))
#define SE_MUL(a, b, c) sevaluator_result_mul(&(a), &(b), &(c))
#define SE_DIV(a, b, c) sevaluator_result_div(&(a), &(b), &(c))
#define SE_NEG(a, b) sevaluator_result_neg(&(a), &(b))
#define SE_SQRT(a, b) sevaluator_result_sqrt(&(a), &(b))

#define SE_CHECK_ZERO(a) sevaluator_result_check_zero(&(a))

%}

%union {
    FullResult result;
    mpq_t v_rational;
    mpz_t v_integer;
    mpf_t v_float;
}

%token<v_integer> INTEGER;
%token<v_rational> RATIONAL;
%token<v_float> FLOAT;
%token PLUS MINUS MULTIPLY DIVIDE POWER MOD SQRT LEFT RIGHT F_LEFT F_RIGHT COMMA FUNC_HIST FUNC_RANDOM FACTORIAL ANS
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD

%type<result> expr
%type<result> fact
%type<result> root
%type<result> number
%type<result> literal

%start calc

%destructor { SE_DESTROY($$); } <result>
%destructor { mpq_clear($$); } <v_rational>
%destructor { mpz_clear($$); } <v_integer>
%destructor { mpf_clear($$); } <v_float>


/* %parse-param { ResultType *result_type } { Result *result } */

%%
calc: expr {
        SE_COPY(final_result, $1);
        SE_DESTROY($1); 
    }
expr: expr PLUS fact { SE_ADD($$, $1, $3); SE_DESTROY($1); SE_DESTROY($3); }
    | expr MINUS fact { SE_SUB($$, $1, $3); SE_DESTROY($1); SE_DESTROY($3); }
    | fact { SE_COPY($$, $1); SE_DESTROY($1); }
    ;
fact: fact MULTIPLY root { SE_MUL($$, $1, $3); SE_DESTROY($1); SE_DESTROY($3); }
    | fact DIVIDE root {
        if (SE_CHECK_ZERO($3) == 0) {
            fprintf(stderr, "divide 0\n");
            error_type = E_DIV_ZERO;
            YYABORT;
        }
        SE_DIV($$, $1, $3);
        SE_DESTROY($1);
        SE_DESTROY($3);
    }
    | root { SE_COPY($$, $1); SE_DESTROY($1); }
    | MINUS fact { SE_NEG($$, $2); SE_DESTROY($2); }
    | PLUS fact { SE_COPY($$, $2); SE_DESTROY($2); }
    ;
root: SQRT root {
        if (SE_CHECK_ZERO($2) < 0) {
            fprintf(stderr, "negative root\n");
            error_type = E_NEGTIVE_ROOT;
            YYABORT;
        }
        SE_SQRT($$, $2);
        SE_DESTROY($2);
    }
    | number { SE_COPY($$, $1); SE_DESTROY($1); }
    ;
number:
    LEFT expr RIGHT { SE_COPY($$, $2); SE_DESTROY($2); }
    | literal { SE_COPY($$, $1); }
    ;
literal: 
    ANS {
        if (mode == NO_HISTORY) { error_type = E_NO_HISTORY; YYABORT; }
        const char *hist_result = sevaluator_history_get(history_list, history_list->len-1);
        if (!hist_result) {
            YYABORT;
        }
        ResultType type = sevaluator_history_get_type(history_list, history_list->len-1);
        sevaluator_result_init_str(&$$, type, hist_result);
    }
    | FUNC_HIST F_LEFT INTEGER F_RIGHT { 
        if (mode == NO_HISTORY) { error_type = E_NO_HISTORY; YYABORT; } 
        if (mpz_cmp_ui($3, 1000000) > 0) {
            mpz_clear($3);
            error_type = E_NO_HISTORY;
            YYABORT;
        }
        int index = (int) mpz_get_ui($3);
        const char *hist_result = sevaluator_history_get(history_list, index);
        if (!hist_result) {
            mpz_clear($3);
            error_type = E_NO_HISTORY; 
            YYABORT;
        }
        ResultType type = sevaluator_history_get_type(history_list, index);
        sevaluator_result_init_str(&$$, type, hist_result);
    }
    | FUNC_RANDOM F_LEFT F_RIGHT {
        unsigned long temp = gmp_urandomb_ui(random_state, 8);
        sevaluator_result_init(&$$, R_INT);
        mpz_set_ui($$.result.v_int, temp);
    }
    | RATIONAL { sevaluator_result_init(&$$, R_RAT); mpq_set($$.result.v_rat, $1); mpq_clear($1); }
    | INTEGER { sevaluator_result_init(&$$, R_INT); mpz_set($$.result.v_int, $1); mpz_clear($1); }
    ;

%%

void init_random_state() {
    gmp_randinit_default(random_state);
    unsigned long seed;
    getrandom(&seed, sizeof(seed), GRND_RANDOM);
    gmp_randseed_ui(random_state, seed);
}

ErrorType sevaluator_calc(const char *input, char **output, HistoryList *list, size_t float_digits) {
    init_random_state();
    error_type = E_OK;
    if (!list) {
        mode = NO_HISTORY;
    } else {
        mode = DEFAULT;
    }
    history_list = list;
    yy_scan_string(input);
    int error = yyparse();
    if (error) {
        *output = NULL;
    } else {
        *output = sevaluator_result_get_str(&final_result, float_digits);
        if (mode != NO_HISTORY) {
            sevaluator_history_push(history_list, *output, final_result.result_type);
            sevaluator_result_destroy(&final_result);
        }
    }
    
    history_list = NULL;
    yylex_destroy();
    gmp_randclear(random_state);

    return error_type;
}

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
    if (error_type == E_OK) {
        error_type = E_SYNTAX;
    }
}