#include <unistd.h>

/*
 * This is a OpenBSD sandboxing function. We stub it to
 * be compatible with OpenBSD programs.
 */
int pledge(const char *promises, const char *execpromises)
{
    return 0;
}