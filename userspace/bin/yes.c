/* Yes. */
#include "types.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc > 1)
        for (;;)
            printf(stdout, argv[1]);
    else
        for (;;)
            printf(stdout, "y");
    return 0;
}