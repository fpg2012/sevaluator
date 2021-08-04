# Sevaluator

> Still under development

A simple expression evaluator, generated with flex & bison. Sevaluator invokes GMP library to support large number.

usage example (see test.c)

```c
#include "sevaluator.h"
#inculde <stdio.h>
#include <stdlib.h>

int main() {
    char *output;
	calc("-12-3*4/2", &output);
	printf("%s\n", output);
	free(output);
	return 0;
}
```

Expected output: -18
