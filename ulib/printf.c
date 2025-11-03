#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

void putc(FILE* stream, char c)
{
    if (!stream)
        return;

    if (write(stream->fd, &c, 1) != 1) {
        stream->flags |= FILE_ERR;
    }
}

void printint(FILE* stream, int xx, int base, int sgn)
{
    static char digits[] = "0123456789ABCDEF";
    char buf[16];
    int i = 0;
    int neg = 0;
    unsigned int x;

    if (sgn && xx < 0) {
        neg = 1;
        x = (unsigned int)(-xx);
    } else {
        x = (unsigned int)xx;
    }

    do {
        buf[i++] = digits[x % base];
        x /= base;
    } while (x != 0);

    if (neg)
        buf[i++] = '-';

    while (--i >= 0)
        putc(stream, buf[i]);
}

/*
 * Print to console (always fd 0, 1, 2)
 * Here we'll use fd 1, but you can change it
 * to anything and not feel any impact whatsoever
 */
void printf(const char* fmt, ...)
{
    char* s;
    int c, state;
    va_list ap;
    FILE* stream = stdout;

    va_start(ap, fmt);
    state = 0;

    for (int i = 0; fmt[i]; i++) {
        c = fmt[i] & 0xFF;

        if (state == 0) {
            if (c == '%') {
                state = '%';
            } else {
                putc(stream, c); // FILE* instead of fd
            }
        } else if (state == '%') {
            switch (c) {
            case 'd':
                printint(stream, va_arg(ap, int), 10, 1);
                break;
            case 'x':
            case 'p':
                printint(stream, va_arg(ap, int), 16, 0);
                break;
            case 's':
                s = va_arg(ap, char*);
                if (!s)
                    s = "(null)";
                while (*s)
                    putc(stream, *s++);
                break;
            case 'c':
                putc(stream, va_arg(ap, int));
                break;
            case '%':
                putc(stream, '%');
                break;
            default:
                putc(stream, '%');
                putc(stream, c);
                break;
            }
            state = 0;
        }
    }

    va_end(ap);
}

void puts(const char* str) { printf(str); }

long strtonum(const char* numstr, long minval, long maxval, const char** errstr)
{
    char* end;
    long val = strtol(numstr, &end, 10);

    if (end == numstr || *end != '\0') {
        if (errstr)
            *errstr = "invalid!";
        return 0;
    }
    if (val < minval) {
        if (errstr)
            *errstr = "too small!";
        return minval;
    }
    if (val > maxval) {
        if (errstr)
            *errstr = "too large!";
        return maxval;
    }
    if (errstr)
        *errstr = NULL;
    return val;
}