#include "sevaluator.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *output;
    char *output2;
    HistoryList *hist_list = sevaluator_history_create();
    int error = sevaluator_calc("-12+3*5", &output, hist_list);
    if (error) {
        printf("error\n");
        return 1;
    }
    printf("%s\n", output);
    error = sevaluator_calc("1 *100 - 10", &output2, hist_list);
    if (error) {
        printf("error\n");
        return 1;
    }
    printf("%s\n", output2);
    error = sevaluator_calc("hist[0]", &output2, hist_list);
    if (error) {
        printf("error\n");
        return 1;
    }
    printf("%s\n", output2);
    free(output);
    free(output2);
    sevaluator_history_destory(hist_list);
    return 0;
}