#ifndef _DIRENT_H_
#define _DIRENT_H_

struct dirent {
    uint ino;
    char name[256];
};

typedef struct DIR DIR;

DIR* opendir(const char* path);
struct dirent* readdir(DIR* d);
int closedir(DIR* d);

#endif