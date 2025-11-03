#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include <stdio.h>
#include <unistd.h>
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

int putchar(int c)
{
    char ch = c;
    return write(1, &ch, 1);
}

int isatty(int fd) { return (fd >= 0 && fd <= 2); }

int strcspn(const char* s, const char* reject)
{
    const char *p, *r;
    for (p = s; *p; p++) {
        for (r = reject; *r; r++)
            if (*p == *r)
                return p - s;
    }
    return p - s;
}

int strlcat(char* dst, const char* src, size_t size)
{
    const char* s = src;
    char* d = dst;
    size_t n = size;
    size_t dlen;

    while (n-- != 0 && *d != '\0')
        d++;
    dlen = d - dst;
    n = size - dlen;

    if (n == 0)
        return dlen + strlen(src);

    while (*s != '\0') {
        if (n != 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';

    return dlen + (s - src);
}

void* memcpy(void* dst, const void* src, unsigned n)
{
    char* d = dst;
    const char* s = src;
    for (unsigned i = 0; i < n; i++)
        d[i] = s[i];
    return dst;
}

int snprintf(char* buf, int size, const char* fmt, ...)
{
    int i = 0;
    va_list ap;
    va_start(ap, fmt);

    for (const char* p = fmt; *p; p++) {
        if (*p != '%') {
            if (i < size - 1)
                buf[i++] = *p;
            continue;
        }

        p++; /* skip % */
        if (!*p)
            break;

        if (*p == 'd') {
            int val = va_arg(ap, int);
            char numbuf[16];
            int j = 0;
            int neg = 0;

            if (val < 0) {
                neg = 1;
                val = -val;
            }

            if (val == 0) {
                numbuf[j++] = '0';
            } else {
                while (val > 0) {
                    numbuf[j++] = '0' + (val % 10);
                    val /= 10;
                }
            }

            if (neg)
                numbuf[j++] = '-';

            for (int k = j - 1; k >= 0; k--)
                if (i < size - 1)
                    buf[i++] = numbuf[k];

        } else if (*p == 'x') {
            unsigned int val = va_arg(ap, unsigned int);
            char numbuf[16];
            int j = 0;

            if (val == 0) {
                numbuf[j++] = '0';
            } else {
                while (val > 0) {
                    int digit = val & 0xF;
                    numbuf[j++] = digit < 10 ? '0' + digit : 'a' + (digit - 10);
                    val >>= 4;
                }
            }

            for (int k = j - 1; k >= 0; k--)
                if (i < size - 1)
                    buf[i++] = numbuf[k];

        } else if (*p == 's') {
            char* s = va_arg(ap, char*);
            while (*s && i < size - 1)
                buf[i++] = *s++;

        } else if (*p == 'c') {
            char c = (char)va_arg(ap, int);
            if (i < size - 1)
                buf[i++] = c;

        } else {
            if (i < size - 1)
                buf[i++] = *p;
        }
    }

    buf[i] = '\0';
    va_end(ap);
    return i;
}
