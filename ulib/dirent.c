#include <sys/stat.h>
#include <stdio.h>
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

    DIR* d = (DIR*)malloc(sizeof(DIR));
    if (!d) {
        close(fd);
        return 0;
    }
    d->fd = fd;
    d->offset = 0;
    return d;
}

struct dirent* readdir(DIR* d)
{
    struct dirent* de = &d->de;
    int n;

    while ((n = read(d->fd, (char*)de, sizeof(struct dirent)))
        == sizeof(struct dirent)) {
        if (de->inum == 0)
            continue;

        de->name[DIRSIZ] = 0;
        return de;
    }
}

int closedir(DIR* d)
{
    if (!d)
        return -1;

    int ret = close(d->fd);
    free(d);
    return (ret < 0) ? -1 : 0;
}