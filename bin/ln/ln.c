#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: ln target linkname\n");
        return 1;
    }

    if(symlink(argv[1], argv[2]) < 0) {
        printf("ln: failed to create symlink\n");
        return 1;
    }

    return 0;
}
