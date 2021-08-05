#ifndef SEVALUATOR 
#define SEVALUATOR
#include <gmp.h>
#include "sevaluator_history.h"
#include "sevaluator_result.h"

typedef enum mode {
    DEFAULT,
    NO_HISTORY,
} Mode;

int sevaluator_calc_no_history(const char *input, char **output);
int sevaluator_calc(const char *input, char **output, HistoryList *list);

#endif