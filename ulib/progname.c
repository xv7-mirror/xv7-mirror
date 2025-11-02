#include <stdio.h>

const char* progname = "unknown";

const char* getprogname(void) { return progname; }

void setprogname(const char* name) { progname = name; }