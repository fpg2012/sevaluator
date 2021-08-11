#ifndef SEVALUATOR 
#define SEVALUATOR
#include <gmp.h>
#include "sevaluator_history.h"
#include "sevaluator_result.h"

typedef enum mode {
    DEFAULT,
    NO_HISTORY,
} Mode;

typedef enum error_type {
    E_OK,
    E_SYNTAX,
    E_DIV_ZERO,
    E_NEGTIVE_ROOT,
    E_NO_HISTORY,
    E_TYPE,
    E_INVALID_PARAMETER
} ErrorType;

ErrorType sevaluator_calc(const char *input, char **output, HistoryList *list, size_t float_digits);

#endif