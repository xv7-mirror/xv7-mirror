#ifndef _STDIO_H_
#define _STDIO_H_

#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/utsname.h>

#define _IONBF 0 /* non-buffered mode */
#define BUFSIZ 512
#define DIRSIZ 512

typedef struct {
    int fd;
    char buf[BUFSIZ];
    int bufpos;
    int buflen;
    int flags;
} FILE;

int uname(struct utsname* buf);

#define FILE_ERR 0x1
#define FILE_EOF 0x2

extern FILE* stdout;
extern FILE* stdin;
extern FILE* stderr;

extern int optind;
extern char* optarg;

#define EOF (-1)

void printf(const char* fmt, ...);
void fprintf(FILE* stream, const char* fmt, ...);
char* gets(char*, int max);
int getcwd(char* buf, int size);
int vsscanf(char* input, const char* fmt, va_list ap);
int sscanf(const char* str, const char* fmt, ...);
int scanf(const char* fmt, ...);
FILE* fopen(const char* path, const char* mode);
int fwrite(void* ptr, int size, int nmemb, FILE* stream);
int fread(void* ptr, int size, int nmemb, FILE* stream);
int fclose(FILE* stream);
int fputs(const char* s, FILE* stream);
int fileno(FILE* stream);
void clearerr(FILE* stream);
int ferror(FILE* stream);
int setvbuf(FILE* stream, char* buf, int mode, int size);
int getopt(int argc, char** argv, char* opts);
int getc(FILE* stream);
char getchar();
void putc(FILE* stream, char c);
void printint(FILE* stream, int xx, int base, int sgn);
const char* getprogname(void);
int setprogname(char* src);
void puts(const char* str);
int snprintf(char* buf, int size, const char* fmt, ...);
long strtonum(
    const char* numstr, long minval, long maxval, const char** errstr);
char* fgets(char* buf, int size, FILE* stream);

#endif
