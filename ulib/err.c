#include "err.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

void err(int eval, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    putchar('\n');
    exit(0);
}

void errx(int eval, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    putchar('\n');
    exit(0);
}

void warnx(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    putchar('\n');
}

void warn(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    putchar('\n');
}