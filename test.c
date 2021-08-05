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

void test_result() {
    FullResult a, b, c;
    sevaluator_result_init_str(&a, R_FLT, "-0.1");
    sevaluator_result_init_str(&b, R_FLT, "0.2");
    sevaluator_result_mul(&c, &a, &b);
    char *temp = sevaluator_result_get_str(&c, 10);
    printf("%s\n", temp);
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

    test_result();
    return 0;
}