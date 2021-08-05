# Sevaluator

> Still under development

A simple expression evaluator, generated with flex & bison. Sevaluator invokes GMP library to support large number.

usage example (see test.c):

```c
#include "sevaluator.h"
#inculde <stdio.h>
#include <stdlib.h>

int main() {
    char *output;
	HistoryList *hist_list = sevaluator_history_create();

	sevaluator_calc("10 + 3 * 3 / 2", &output, list, 5)
	printf("%s\n", output);

	sevaluator_calc("-√(216+3*123/√(1243-452))", &output, list, 5)
	printf("%s\n", output);

	sevaluator_calc("(ans) * 2", &output, list, 5)
	printf("%s\n", output);

	sevaluator_calc("hist[0] * 2", &output, list, 5)
	printf("%s\n", output);

	free(output);
	sevaluator_history_destory(hist_list);
	return 0;
}
```

Expected output:

```
29/2
-15.137
-30.274
29
```
