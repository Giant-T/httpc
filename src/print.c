#include "print.h"

#include <stdarg.h>
#include <stdio.h>

void print_err(const char *format, ...) {
    fprintf(stderr, "\033[31mERROR\033[0m: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
