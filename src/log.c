#include "log.h"

#include <stdarg.h>
#include <stdio.h>

void log_err(const char *format, ...) {
    fprintf(stderr, "\033[31mERROR\033[0m: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

void log_info(const char *format, ...) {
    fprintf(stderr, "\033[32mINFO\033[0m: ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
