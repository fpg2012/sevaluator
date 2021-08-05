#include "sevaluator.h"
#include <stdio.h>
#include <stdlib.h>

int test(const char *expr, HistoryList *list) {
    char *output;
    int error = sevaluator_calc(expr, &output, list);
    if (error) {
        printf("error\n");
        return 1;
    }
    printf("%s\n", output);
    free(output);
}

int main() {
    HistoryList *hist_list = sevaluator_history_create();

    test("1 *100 - 10", hist_list);
    test("hist[0]", hist_list);
    test("hist[1] + hist[0]", hist_list);
    test("random[]", hist_list);
    test("hist[100]", hist_list);
    test("hist[3]", hist_list);

    sevaluator_history_destory(hist_list);
    return 0;
}