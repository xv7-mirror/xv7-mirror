#include "types.h"
#include "stat.h"
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct DIR {
    int fd;
    struct dirent de;
    int offset;
};

DIR* opendir(const char* path)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return 0;

    DIR* d = malloc(sizeof(DIR));
    if (!d)
        return 0;

    d->fd = fd;
    d->offset = 0;
    return d;
}

struct dirent* readdir(DIR* d)
{
    struct dirent* de = &d->de;
    struct dirent buf;

    /*
     * Read sizeof(dirent) bytes from current
     * offset
     */
    int n = read(d->fd, (char*)&buf, sizeof(buf));
    if (n != sizeof(buf))
        return 0;

    d->offset += sizeof(buf);
    *de = buf;
    return de;
}

int closedir(DIR* d)
{
    if (!d)
        return -1;
    close(d->fd);
    free(d);
    return 0;
}