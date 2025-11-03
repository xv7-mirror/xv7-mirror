#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;

char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
uint strlen(const char*);
void* memset(void*, int, uint);
void strcat(char* dest, const char* src);
int strlcat(char* dst, const char* src, size_t size);
int strcspn(const char* s, const char* reject);
long strtol(const char* nptr, char** endptr, int base);
size_t strlcpy(char *dst, const char *src, size_t dsize);

#endif