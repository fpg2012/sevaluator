#include "sevaluator.h"
#include <stdio.h>
#include <stdlib.h>

int test(const char *expr, HistoryList *list) {
    char *output;
    int error = sevaluator_calc(expr, &output, list, 5);
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

    test("10 + 3", hist_list);
    test("10 * 3", hist_list);
    test("10 - 3", hist_list);
    test("10 / 3", hist_list);

    test("-√2", hist_list);
    test("--√√4", hist_list);

    test("(1)", hist_list);
    test("((1))", hist_list);
    test("-(1)", hist_list);
    test("(12+13)", hist_list);
    test("(12-23)", hist_list);
    test("(23-12)", hist_list);
    test("(12 * 4)", hist_list);
    test("(12 / 5)", hist_list);
    test("(-1)", hist_list);
    test("-√(216+3*123/√(1243-452))", hist_list);

    test("10 / 0", hist_list);
    test("-√(2-16)", hist_list);

    test("(ans) * 2", hist_list);
    test("hist[7]", hist_list);

    sevaluator_history_destory(hist_list);

    test_result();
    return 0;
}