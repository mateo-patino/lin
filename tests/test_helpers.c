#include "test_helpers.h"

#include <stdarg.h>
#include <stdio.h>


void perr(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, fmt, args);
    va_end(args);
}
