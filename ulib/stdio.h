#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

typedef struct {
  int fd;
} FILE;

extern FILE *stdout;
extern FILE *stderr;

int printf(const char *fmt, ...);
char* gets(char*, int max);

#endif