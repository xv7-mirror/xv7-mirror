#include "stat.h"
#include "types.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int i;

    if (argc < 2) {
        puts("Usage: kill pid...\n");
        return 0;
    }
    for (i = 1; i < argc; i++)
        kill(atoi(argv[i]));
    return 0;
}
