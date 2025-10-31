/* Yes. */
#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
	if (argc > 1)
		for (;;)
            printf(1, argv[1]);
	else
		for (;;)
            printf(1, "y");
    exit();
}