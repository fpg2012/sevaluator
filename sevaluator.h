#ifndef SEVALUATOR 
#define SEVALUATOR
#include <gmp.h>
#include "sevaluator_history.h"

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

typedef enum mode {
    DEFAULT,
    NO_HISTORY,
} Mode;

int sevaluator_calc_no_history(const char *input, char **output);
int sevaluator_calc(const char *input, char **output, HistoryList *list);

#endif