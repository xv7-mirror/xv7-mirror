#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv)
{
    printf("%d secs after boot\n", uptime());
    return 0;
}
