#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

FILE __stdout_file = { .fd = 1 };
FILE* stdout = &__stdout_file;

FILE __stdin_file = { .fd = 1 };
FILE* stdin = &__stdin_file;

FILE __stderr_file = { .fd = 1 };
FILE* stderr = &__stderr_file;

int optind = 1;
char* optarg;
int optopt = 0;

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

int fputs(const char* s, FILE* stream)
{
    if (!s || !stream)
        return -1;
    size_t len = strlen(s);
    int written = write(stream->fd, s, (int)len);
    if (written < 0)
        return -1;
    return 0;
}

int ferror(FILE* stream)
{
    /*
     * Treat null as an error.
     */
    if (!stream)
        return 1;

    return (stream->flags & FILE_ERR) != 0;
}

char* fgets(char* buf, int size, FILE* stream)
{
    int i;

    for (i = 0; i < size - 1; i++) {
        char c;
        int n = read(stream->fd, &c, 1);
        if (n < 1)
            break;
        buf[i++] = c;
        if (c == '\n')
            break;
    }
    buf[i] = '\0';
    return (i == 0) ? 0 : buf;
}

void clearerr(FILE* stream)
{
    if (stream)
        stream->flags &= ~(FILE_ERR | FILE_EOF);
}

int fileno(FILE* stream)
{
    if (!stream)
        return -1;
    return stream->fd;
}

int setvbuf(FILE* stream, char* buf, int mode, int size)
{
    /*
     * Don't do anything. We don't need that
     * yet
     */
    return 0;
}

int getopt(int argc, char* argv[], const char* optstring)
{
    static int pos = 1;

    if (optind >= argc)
        return -1;

    char* arg = argv[optind];

    if (pos == 1) {
        if (arg[0] != '-' || arg[1] == '\0')
            return -1;
        if (strcmp(arg, "--") == 0) {
            optind++;
            return -1;
        }
    }

    char c = arg[pos];
    const char* decl = strchr(optstring, c);

    if (decl == NULL) {
        optopt = c;
        if (arg[++pos] == '\0') {
            pos = 1;
            optind++;
        }
        return '?';
    }

    if (decl[1] == ':') {
        if (arg[pos + 1] != '\0') {
            optarg = &arg[pos + 1];
            pos = 1;
            optind++;
        } else if (optind + 1 < argc) {
            optarg = argv[optind + 1];
            pos = 1;
            optind += 2;
        } else {
            optopt = c;
            pos = 1;
            optind++;
            return '?';
        }
        return c;
    } else {
        optarg = 0;
        if (arg[++pos] == '\0') {
            pos = 1;
            optind++;
        }
        return c;
    }
}

int getc(FILE* stream)
{
    if (!stream)
        return 0;

    if (stream->bufpos >= stream->buflen) {
        int n = read(stream->fd, stream->buf, BUFSIZ);
        if (n < 0) {
            stream->flags |= FILE_ERR;
            return -1;
        } else if (n == 0) {
            stream->flags |= FILE_EOF;
            return -1;
        }
        stream->buflen = n;
        stream->bufpos = 0;
    }

    return (unsigned char)stream->buf[stream->bufpos++];
}

void fprintf(FILE* stream, const char* fmt, ...)
{
    char* s;
    int c, state;
    va_list ap;
    va_start(ap, fmt);
    state = 0;
    if (!stream)
        return;

    for (int i = 0; fmt[i]; i++) {
        c = fmt[i] & 0xFF;

        if (state == 0) {
            if (c == '%') {
                state = '%';
            } else {
                putc(stream, c);
            }
        } else if (state == '%') {
            switch (c) {
            case 'd':
                printint(stream, va_arg(ap, int), 10, 1);
                break;
            case 'x':
            case 'p':
                printint(stream, va_arg(ap, int), 16, 0);
                break;
            case 's':
                s = va_arg(ap, char*);
                if (!s)
                    s = "(null)";
                while (*s)
                    putc(stream, *s++);
                break;
            case 'c':
                putc(stream, va_arg(ap, int));
                break;
            case '%':
                putc(stream, '%');
                break;
            default:
                putc(stream, '%');
                putc(stream, c);
                break;
            }
            state = 0;
        }
    }

    va_end(ap);
}