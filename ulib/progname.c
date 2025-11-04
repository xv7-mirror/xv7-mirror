#include <stdio.h>
#include <unistd.h>

char progname[100];

const char* getprogname(void)
{
    kgetprogname(progname);
    return progname;
}

int setprogname(char* src)
{
    ksetprogname(src);
    return 0;
}
