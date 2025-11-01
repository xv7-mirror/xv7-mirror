#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

typedef unsigned int uint;

char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
uint strlen(const char*);
void* memset(void*, int, uint);
void strcat(char* dest, const char* src);

#endif