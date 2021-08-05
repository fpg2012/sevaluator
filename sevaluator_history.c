#include "sevaluator_history.h"
#include <stdio.h>

static HistortyNode* _get_recent_node(HistoryList *list) {
    return list->first_node->next_node;
}

static void _destroy_node(HistortyNode *node) {
    free(node->p);
    free(node);
}

HistoryList *sevaluator_history_create() {
    HistoryList *list = (HistoryList*) malloc(sizeof(HistoryList));

    list->first_node = (HistortyNode*) malloc(sizeof(HistortyNode));

    list->first_node->next_node = NULL;
    list->first_node->p = NULL;

    list->len = 0;
    return list;
}

void sevaluator_history_destory(HistoryList *list) {
    HistortyNode *temp;
    while (temp = _get_recent_node(list)) {
        list->first_node->next_node = temp->next_node;
        _destroy_node(temp);
    }
    free(list->first_node);
    free(list);
}

const char *sevaluator_history_get(HistoryList *list, int k) {
    if (k >= list->len) {
        return NULL;
    }
    HistortyNode *current_node = list->first_node;
    for (int i = list->len-1; i > k; --i) {
        current_node = current_node->next_node;
    }
    return current_node->next_node->p;
}

int sevaluator_history_get_length(HistoryList *list) {
    return list->len;
}

void sevaluator_history_push(HistoryList *list, const char *p) {
    HistortyNode *node = (HistortyNode*) malloc(sizeof(HistortyNode));

    char *str = (char*) malloc(strlen(p) + 1);
    strcpy(str, p);

    node->p = str;
    node->next_node = list->first_node->next_node;

    list->first_node->next_node = node;
    list->len++;
}

void sevaluator_history_print(HistoryList *list) {
    HistortyNode *cur = list->first_node->next_node;
    while (cur) {
        printf("%s\n", cur->p);
        cur = cur->next_node;
    }
}