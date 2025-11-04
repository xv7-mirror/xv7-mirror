#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <stddef.h>
#include <sys/cdefs.h>

int fork(void);
int exit(int) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, const void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(const char*, int);
int mknod(const char*, short, short);
int unlink(const char*);
int fstat(int fd, struct stat*);
int link(const char*, const char*);
int mkdir(const char*);
int chdir(const char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int getppid(void);
int ioctl(int, int, void*);
int sync();
int putchar(int c);
int kgetprogname(char* dst);
int ksetprogname(char* src);

int isatty(int fd);
int pledge(const char* promises, const char* execpromises);

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#endif
