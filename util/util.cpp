#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void ifError(bool state, const char* errMsg) {
    if (state) {
        perror(errMsg);
        exit(EXIT_FAILURE);
    }
}