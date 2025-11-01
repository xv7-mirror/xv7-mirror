/*
 * Basic file to test out xv7 libc
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() { 
    printf("Hello from stdio\n");
    /*
     * What you would do on old xv6 is:
     * printf(1, "Hello from stdio\n");
     * exit();
     * I think you can figure out the issue in
     * doing it like that yourself.
     */
    return 0;
}
