#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>

typedef struct {
    int fd;
} FILE;

#define O_RDONLY 0x000
#define O_WRONLY 0x001
#define O_RDWR 0x002
#define O_CREATE 0x200

extern FILE* stdout;

int printf(const char* fmt, ...);
void fprintf(int fd, const char* fmt, ...);
char* gets(char*, int max);
int getcwd(char* buf, int size);
int scanf(const char* fmt, void* arg);
FILE* fopen(const char* path, const char* mode);
int fwrite(void* ptr, int size, int nmemb, FILE* stream);
int fread(void* ptr, int size, int nmemb, FILE* stream);
int fclose(FILE* stream);
int fputs(const char* s, FILE* stream);

#endif