#include <stdio.h>
#include <string.h>

int uname(struct utsname *buf) {
    strcpy(buf->sysname, "xv7");
    strcpy(buf->nodename, "xv7");
    strcpy(buf->release, "0.0.1");
    strcpy(buf->version, "");
    strcpy(buf->machine, "x86");
    return 0;
}

