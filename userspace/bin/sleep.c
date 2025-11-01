#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void usage()
{
    printf("Usage: sleep ticks\n");
}

int main(int argc, char **argv)
{
    int c, n;
    char *s;

    n = 0;
    if (argc < 2) {
        usage();
        return 1;
    }
    s = argv[1];
    while ((c = *s++)) {
        if (c < '0' || c > '9') {
            printf("bad character\n");
            return 1;
        }
        n = n * 10 + c - '0';
    }

    sleep(n);
    return 0;
}
