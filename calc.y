%{
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpfr.h>
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
#define SE_MOD(a, b, c)  sevaluator_result_mod(&(a), &(b), &(c))
#define SE_LN(a, b) sevaluator_result_ln(&(a), &(b))
#define SE_LOG2(a, b) sevaluator_result_log2(&(a), &(b))
#define SE_LOG10(a, b) sevaluator_result_log10(&(a), &(b))
#define SE_EXP(a, b) sevaluator_result_exp(&(a), &(b))
#define SE_SIN(a, b) sevaluator_result_sin(&(a), &(b))
#define SE_COS(a, b) sevaluator_result_cos(&(a), &(b))
#define SE_TAN(a, b) sevaluator_result_tan(&(a), &(b))
#define SE_COT(a, b) sevaluator_result_cot(&(a), &(b))
#define SE_SEC(a, b) sevaluator_result_sec(&(a), &(b))
#define SE_CSC(a, b) sevaluator_result_csc(&(a), &(b))
#define SE_ASIN(a, b)
#define SE_ACOS(a, b)
#define SE_ATAN(a, b)
#define SE_POW(a, b, c) sevaluator_result_pow(&(a), &(b), &(c))

#define SE_CHECK_ZERO(a) sevaluator_result_check_zero(&(a))

%}

%union {
    FullResult result;
    mpq_t v_rational;
    mpz_t v_integer;
    mpfr_t v_float;
}

%token<v_integer> INTEGER;
%token<v_rational> RATIONAL;
%token<v_float> FLOAT;
%token PLUS MINUS MULTIPLY DIVIDE MOD SQRT POWER
%token LEFT RIGHT F_LEFT F_RIGHT COMMA
%token FUNC_HIST FUNC_RANDOM FACTORIAL ANS
%token FUNC_ROOT
%token FUNC_LOG2 FUNC_LOG10 FUNC_LN FUNC_LOG FUNC_EXP
%token FUNC_SIN FUNC_COS FUNC_TAN FUNC_COT FUNC_CSC FUNC_SEC
%token FUNC_ASIN FUNC_ACOS FUNC_ATAN
%token FUNC_ABS
%token CONST_PI CONST_E
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
%destructor { mpfr_clear($$); } <v_float>


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
fact: fact MOD root {
        if (SE_CHECK_ZERO($3) == 0) {
            fprintf(stderr, "mod 0\n");
            error_type = E_DIV_ZERO;
            YYABORT;
        }
        if ($1.result_type != R_INT || $3.result_type != R_INT) {
            fprintf(stderr, "mod operands should be integers\n");
            error_type = E_TYPE;
            YYABORT;
        }
        SE_MOD($$, $1, $3);
        SE_DESTROY($1);
        SE_DESTROY($3);
    }
    | fact MULTIPLY root { SE_MUL($$, $1, $3); SE_DESTROY($1); SE_DESTROY($3); }
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
root:
    SQRT root {
        if (SE_CHECK_ZERO($2) < 0) {
            fprintf(stderr, "negative root\n");
            error_type = E_NEGTIVE_ROOT;
            YYABORT;
        }
        SE_SQRT($$, $2);
        SE_DESTROY($2);
    }
    | number POWER root {
        if (SE_CHECK_ZERO($1) == 0 && SE_CHECK_ZERO($3) < 0) {
            fprintf(stderr, "divide 0\n");
            error_type = E_DIV_ZERO;
            YYABORT;
        }
        if ($3.result_type == R_RAT &&
            SE_CHECK_ZERO($1) < 0 &&
            (mpz_even_p(mpq_denref($3.result.v_rat)) ||
            mpz_cmp_ui(mpq_denref($3.result.v_rat), ULONG_MAX) >= 0)
        ) {
            fprintf(stderr, "negative root\n");
            error_type = E_NEGTIVE_ROOT;
            YYABORT;
        }
        else if ($3.result_type == R_FLT &&
            SE_CHECK_ZERO($1) < 0
        ) {
            fprintf(stderr, "negative root\n");
            error_type = E_NEGTIVE_ROOT;
            YYABORT;
        }
        SE_POW($$, $1, $3);
        SE_DESTROY($1);
        SE_DESTROY($3);
    }
    | number { SE_COPY($$, $1); SE_DESTROY($1); }
    ;
number:
    LEFT expr RIGHT { SE_COPY($$, $2); SE_DESTROY($2); }
    | FUNC_EXP F_LEFT expr F_RIGHT {
        SE_EXP($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_LN F_LEFT expr F_RIGHT {
        if (SE_CHECK_ZERO($3) < 0) {
            fprintf(stderr, "negative log parameter\n");
            error_type = E_INVALID_PARAMETER;
            YYABORT;
        }
        SE_LN($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_LOG2 F_LEFT expr F_RIGHT {
        if (SE_CHECK_ZERO($3) < 0) {
            fprintf(stderr, "negative log parameter\n");
            error_type = E_INVALID_PARAMETER;
            YYABORT;
        }
        SE_LOG2($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_LOG10 F_LEFT expr F_RIGHT {
        if (SE_CHECK_ZERO($3) < 0) {
            fprintf(stderr, "negative log parameter\n");
            error_type = E_INVALID_PARAMETER;
            YYABORT;
        }
        SE_LOG10($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_SIN F_LEFT expr F_RIGHT {
        SE_SIN($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_COS F_LEFT expr F_RIGHT {
        SE_COS($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_TAN F_LEFT expr F_RIGHT {
        SE_TAN($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_SEC F_LEFT expr F_RIGHT {
        SE_SEC($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_CSC F_LEFT expr F_RIGHT {
        SE_CSC($$, $3);
        SE_DESTROY($3);
    }
    | FUNC_COT F_LEFT expr F_RIGHT {
        SE_COT($$, $3);
        SE_DESTROY($3);
    }
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
    | CONST_PI { sevaluator_result_init(&$$, R_FLT); mpfr_const_pi($$.result.v_flt, MPFR_RNDN); }
    | CONST_E {
        sevaluator_result_init(&$$, R_FLT);
        FullResult fr;
        sevaluator_result_init_str(&fr, R_FLT, "1");
        SE_EXP($$, fr);
        SE_DESTROY(fr);
    }
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