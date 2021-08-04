#ifndef SEVALUATOR_HISTORY
#define SEVALUATOR_HISTORY
#include <string.h>
#include <stdlib.h>

typedef struct history_node {
    char *p;
    struct history_node *next_node;
} HistortyNode;

typedef struct history_list {
    HistortyNode *first_node;
    int len;
} HistoryList;

HistoryList *sevaluator_history_create();

void sevaluator_history_destory(HistoryList *list);

const char *sevaluator_history_get(HistoryList *list, int k);

int sevaluator_history_get_length(HistoryList *list);

void sevaluator_history_push(HistoryList *list, const char *p);

void sevaluator_history_print(HistoryList *list);

#endif