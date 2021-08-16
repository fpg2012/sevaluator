#ifndef SEVALUATOR_RESULT 
#define SEVALUATOR_RESULT
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union result {
    mpz_t v_int;
    mpq_t v_rat;
    mpfr_t v_flt;
} Result;

typedef enum result_type {
    R_INT,
    R_RAT,
    R_FLT,
    R_UNK,
} ResultType;

typedef struct full_result {
    ResultType result_type;
    Result result;
} FullResult;

// attention! this function does NOT call malloc for `result`
void sevaluator_result_init(FullResult *result, ResultType type);

// attention! this function does NOT call malloc for `result`
// does not check str
void sevaluator_result_init_str(FullResult *result, ResultType type, const char *str);

// attention! this function does NOT free memory for `result`
void sevaluator_result_destroy(FullResult *result);

void sevaluator_result_copy(FullResult *result, FullResult *src);

// do nothing if UNABLE to upgrade
void sevaluator_result_upgrade(FullResult *result, ResultType to_upgrade);


/*
    Should not init `result` outside of the following functions.
*/

void sevaluator_result_add(FullResult *result, FullResult *op1, FullResult *op2);

void sevaluator_result_sub(FullResult *result, FullResult *op1, FullResult *op2);

void sevaluator_result_mul(FullResult *result, FullResult *op1, FullResult *op2);

// this function does NOT check whether the `op2` is 0
void sevaluator_result_div(FullResult *result, FullResult *op1, FullResult *op2);

// this function does NOT check whether both `op1` and `op2` are integers
void sevaluator_result_mod(FullResult *result, FullResult *op1, FullResult *op2);

void sevaluator_result_neg(FullResult *result, FullResult *op1);

// this function does NOT check whether `op1` is negative
void sevaluator_result_sqrt(FullResult *result, FullResult *op1);
void sevaluator_result_root_n(FullResult *result, FullResult *op1, unsigned long op2);

void sevaluator_result_ln(FullResult *result, FullResult *op1);
void sevaluator_result_log2(FullResult *result, FullResult *op1);
void sevaluator_result_log10(FullResult *result, FullResult *op1);
void sevaluator_result_exp(FullResult *result, FullResult *op1);

void sevaluator_result_sin(FullResult *result, FullResult *op1);
void sevaluator_result_cos(FullResult *result, FullResult *op1);
void sevaluator_result_tan(FullResult *result, FullResult *op1);
void sevaluator_result_cot(FullResult *result, FullResult *op1);
void sevaluator_result_sec(FullResult *result, FullResult *op1);
void sevaluator_result_csc(FullResult *result, FullResult *op1);
void sevaluator_result_asin(FullResult *result, FullResult *op1);
void sevaluator_result_atan(FullResult *result, FullResult *op1);
void sevaluator_result_acos(FullResult *result, FullResult *op1);

void sevaluator_result_pow(FullResult *result, FullResult *op1, FullResult *op2);

int sevaluator_result_check_zero(FullResult *result);
int sevaluator_result_cmp_si(FullResult *result, long si);

char *sevaluator_result_get_str(FullResult *result, size_t digits);

#ifdef __cplusplus
}
#endif

#endif
