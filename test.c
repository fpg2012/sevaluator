#include "sevaluator.h"
#include <stdio.h>
#include <stdlib.h>

int test(const char *expr, HistoryList *list) {
    char *output;
    int error = sevaluator_calc(expr, &output, list, 5, false);
    if (error) {
        printf("\"%s\" => error\n", expr);
        return 1;
    }
    printf("\"%s\" => %s\n", expr, output);
    free(output);
}

void test_result() {
    FullResult a, b, c;
    sevaluator_result_init_str(&a, R_FLT, "-0.1");
    sevaluator_result_init_str(&b, R_FLT, "0.2");
    sevaluator_result_mul(&c, &a, &b);
    char *temp = sevaluator_result_get_str(&c, 10, false);
    printf("%s\n", temp);
}

int main() {
    HistoryList *hist_list = sevaluator_history_create();

    test("e^2", hist_list);
    test("ans", hist_list);
    test("10 + 3", hist_list);
    test("10 * 3", hist_list);
    test("10 - 3", hist_list);
    test("10 / 3", hist_list);
    
    test("1.0e-3", hist_list);
    test("1E+3", hist_list);
    test("0.2e3", hist_list);

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
    test("1.1 + 10/3", hist_list);
    test("(-1)", hist_list);
    test("-√(216+3*123/√(1243-452))", hist_list);
    test("10 % 3", hist_list);

    // test log
    test("log10[100]", hist_list);
    test("log2[log2[256]]", hist_list);
    test("ln[e*e + e - e]", hist_list);
    test("e", hist_list);

    // test triangular
    test("sin[pi/6]", hist_list);
    test("cos[pi/6]", hist_list);
    test("cot[pi/4 * 3]", hist_list);
    test("tan[pi/4 * 3]", hist_list);
    test("sec[pi/6]", hist_list);
    test("csc[pi/6]", hist_list);
    test("asin[(√2/2)]*4", hist_list);
    test("acos[(√2/2)]/pi", hist_list);
    test("(atan[1] - atan[-1])*2", hist_list);

    // test exp
    test("2^2^3", hist_list);
    test("2^(1/2)", hist_list);
    test("1.1^10", hist_list);
    test("1.1^100", hist_list);
    test("1.1", hist_list);
    test("2^(-1)", hist_list);
    test("(-2)^(1/3)", hist_list);
    test("(-2)^(2/5)", hist_list);
    test("(-2)^(-0.4)", hist_list);
    test("root[-8, 3]", hist_list);

    // test history
    test("(ans) * 2", hist_list);
    test("hist[7]", hist_list);

    // test flt[]
    test("1.1^1000", hist_list);
    test("flt[1.1^1000]", hist_list);
    
    // test random[]
    test("random[]", hist_list);

    // test fact
    test("(1923 % 123 + 1 * 10)!", hist_list);
    test("-3!", hist_list);
    // test("321323!", hist_list);

    // test round
    test("round[5.5]", hist_list);
    test("round[4.5]", hist_list);
    test("round[4.3]", hist_list);
    test("ceil[4.5]", hist_list);
    test("floor[4.5]", hist_list);

    // should be error
    test("10 / 0", hist_list);
    test("-√(2-16)", hist_list);
    test("hist[2.1]", hist_list);
    test("tan[pi/2]", hist_list);
    test("0^(-1)", hist_list);
    test("(-1)^(1/2)", hist_list);
    test("(-1)^(0.5)", hist_list);
    test("root[-8, 2]", hist_list);
    test("root[8, 1.5]", hist_list);
    test("asin[-2]", hist_list);
    test("100000000000000000000000000000000000000000000000!", hist_list);
    test("1.1!", hist_list);
    test("(-1)!", hist_list);

    // token test
    test("g", hist_list);
    test("G", hist_list);
    test("epsilon_0", hist_list);
    test("c", hist_list);
    test("C", hist_list);
    test("N_A", hist_list);

    sevaluator_history_destory(hist_list);

    test_result();
    return 0;
}
