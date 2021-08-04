#include "sevaluator.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *output;
    calc("-12-3*4/2", &output);
    printf("%s\n", output);
    free(output);
    return 0;
}