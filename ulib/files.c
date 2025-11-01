#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE* fopen(const char* path, const char* mode)
{
    int flags = 0;
    if (strcmp(mode, "r") == 0)
        flags = O_RDONLY;
    else if (strcmp(mode, "w") == 0)
        flags = O_WRONLY | O_CREATE;
    else if (strcmp(mode, "rw") == 0)
        flags = O_RDWR | O_CREATE;
    int fd = open(path, flags);
    if (fd < 0)
        return 0;
    FILE* f = malloc(sizeof(FILE));
    f->fd = fd;
    return f;
}

int fread(void* ptr, int size, int nmemb, FILE* stream)
{
    if (!ptr || !stream || size == 0 || nmemb == 0)
        return 0;

    int total_bytes = size * nmemb;
    int n = read(stream->fd, ptr, total_bytes);
    if (n < 0)
        return 0;

    return n / size;
}

int fwrite(void* ptr, int size, int nmemb, FILE* stream)
{
    if (!ptr || !stream || size == 0 || nmemb == 0)
        return 0;

    int total_bytes = size * nmemb;
    int n = write(stream->fd, ptr, total_bytes);
    if (n < 0)
        return 0;

    return n / size;
}

int fclose(FILE* stream)
{
    close(stream->fd);
    return 0;
}