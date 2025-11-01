#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include <stdarg.h>

char* strcpy(char* s, const char* t)
{
    char* os;

    os = s;
    while ((*s++ = *t++) != 0)
        ;
    return os;
}

int strcmp(const char* p, const char* q)
{
    while (*p && *p == *q)
        p++, q++;
    return (uchar)*p - (uchar)*q;
}

uint strlen(const char* s)
{
    int n;

    for (n = 0; s[n]; n++)
        ;
    return n;
}

void* memset(void* dst, int c, uint n)
{
    stosb(dst, c, n);
    return dst;
}

char* strchr(const char* s, char c)
{
    for (; *s; s++)
        if (*s == c)
            return (char*)s;
    return 0;
}

char* gets(char* buf, int max)
{
    int i, cc;
    char c;

    for (i = 0; i + 1 < max;) {
        cc = read(0, &c, 1);
        if (cc < 1)
            break;
        buf[i++] = c;
        if (c == '\n' || c == '\r')
            break;
    }
    buf[i] = '\0';
    return buf;
}

int stat(const char* n, struct stat* st)
{
    int fd;
    int r;

    fd = open(n, O_RDONLY);
    if (fd < 0)
        return -1;
    r = fstat(fd, st);
    close(fd);
    return r;
}

int atoi(const char* s)
{
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

void* memmove(void* vdst, const void* vsrc, int n)
{
    char* dst;
    const char* src;

    dst = vdst;
    src = vsrc;
    while (n-- > 0)
        *dst++ = *src++;
    return vdst;
}

void strcat(char* dest, const char* src)
{
    while (*dest)
        dest++;

    while (*src)
        *dest++ = *src++;

    *dest = '\0';
}

int scanf(const char* fmt, ...)
{
    va_list ap;

    char buf[500];
    char* input;
    const char* f;
    int items_assigned = 0;

    va_start(ap, fmt);

    if (gets(buf, 500) == 0) {
        va_end(ap);
        return -1;
    }

    input = buf;
    f = fmt;

    while (*f) {
        if (*f == ' ' || *f == '\t' || *f == '\n') {
            while (*input == ' ' || *input == '\t' || *input == '\n'
                || *input == '\r')
                input++;
            f++;
            continue;
        }

        if (*f != '%') {
            if (*input == *f) {
                input++;
                f++;
                continue;
            }
            break;
        }

        f++;

        /*
         * Handle format specifiers
         */

        if (*f == 's') {
            char* dest = va_arg(ap, char*);
            int len = 0;

            while (*input == ' ' || *input == '\t' || *input == '\n'
                || *input == '\r')
                input++;

            while (*input && *input != ' ' && *input != '\t' && *input != '\n'
                && *input != '\r') {
                *dest++ = *input++;
                len++;
            }
            *dest = '\0';

            if (len > 0)
                items_assigned++;
        } else if (*f == 'd') {
            int* dest = va_arg(ap, int*);
            char* token_start;
            int sign = 1;

            while (*input == ' ' || *input == '\t' || *input == '\n'
                || *input == '\r')
                input++;

            token_start = input;

            if (*input == '-') {
                sign = -1;
                input++;
            } else if (*input == '+') {
                input++;
            }

            if (*input >= '0' && *input <= '9') {
                *dest = atoi(token_start);

                while (*input >= '0' && *input <= '9')
                    input++;

                items_assigned++;
            } else {
                break;
            }
        } else {
            break;
        }

        f++;
    }

    va_end(ap);

    return items_assigned;
}

int putchar(int c) {
    char ch = c;
    return write(1, &ch, 1);
}