#ifndef SEVALUATOR_RESULT 
#define SEVALUATOR_RESULT
#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>

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

typedef struct full_result {
    ResultType result_type;
    Result result;
} FullResult;

// attention! this function does NOT call malloc for `result`
void sevaluator_result_init(FullResult *result, ResultType type);

// attention! this function does NOT free memory for `result`
void sevaluator_result_destroy(FullResult *result);

// do nothing if UNABLE to upgrade
void sevaluator_result_upgrade(FullResult *result, ResultType to_upgrade);

void sevaluator_result_add(FullResult *result, FullResult *op1, FullResult *op2);

void sevaluator_result_sub(FullResult *result, FullResult *op1, FullResult *op2);

void sevaluator_result_mul(FullResult *result, FullResult *op1, FullResult *op2);

// this function does NOT check whether the `op2` is 0
void sevaluator_result_div(FullResult *result, FullResult *op1, FullResult *op2);

// this function does NOT check whether both `op1` and `op2` are integers
void sevaluator_result_mod(FullResult *result, FullResult *op1, FullResult *op2);

// this function does NOT check whether `op1` is negative
void sevaluator_result_sqrt(FullResult *result, FullResult *op1);

#endif SEVALUATOR_RESULT