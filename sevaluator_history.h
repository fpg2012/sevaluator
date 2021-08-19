#ifndef SEVALUATOR_HISTORY
#define SEVALUATOR_HISTORY
#include <string.h>
#include <stdlib.h>
#include "sevaluator_result.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct history_node {
    char *input;
    FullResult *result;
    struct history_node *next_node;
} HistortyNode;

typedef struct history_list {
    HistortyNode *first_node;
    int len;
} HistoryList;

HistoryList *sevaluator_history_create();

void sevaluator_history_destory(HistoryList *list);

FullResult *sevaluator_history_get(HistoryList *list, int k);

ResultType sevaluator_history_get_type(HistoryList *list, int k);

const char *sevaluator_history_get_input(HistoryList *list, int k);

int sevaluator_history_get_length(HistoryList *list);

void sevaluator_history_push(HistoryList *list, const char *input, FullResult *result);

// void sevaluator_history_print(HistoryList *list);

#ifdef __cplusplus
}
#endif

#endif
