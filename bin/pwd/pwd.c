/*
 * pwd.c - pwd program for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */

#include <stdio.h>

int main(int argc, char* argv[])
{
    char buf[200];
    getcwd(buf, 200);
    printf("%s\n", buf);

    return 0;
}
