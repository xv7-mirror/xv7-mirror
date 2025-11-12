#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>
#include <stdint.h>

typedef unsigned int uint;

char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
int strncmp(const char* p, const char* q, int n);
uint strlen(const char*);
void* memset(void*, int, uint);
int memcmp(const void* s1, const void* s2, int n);
void strcat(char* dest, const char* src);
int strlcat(char* dst, const char* src, size_t size);
int strcspn(const char* s, const char* reject);
long strtol(const char* nptr, char** endptr, int base);
size_t strlcpy(char* dst, const char* src, size_t dsize);
void* memcpy(void* dest, const void* src, unsigned n);
char* strsep(char** s, const char* delim);
char* strtok(char* s, const char* delim);
char* strtok_r(char* s, const char* delim, char** last);

#endif
