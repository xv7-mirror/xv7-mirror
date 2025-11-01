/*
 * touch.c - touch program for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */

#include "fcntl.h"
#include "stat.h"
#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int i;

  if (argc < 2) {
    printf("Usage: touch files...\n");
    return 0;
  }

  for (i = 1; i < argc; i++) {
    int fd = open(argv[i], O_CREATE | O_RDWR);

    if (fd < 0) {
      printf("touch: could not create %s\n", argv[i]);
    } else {
      close(fd);
    }
  }

  return 0;
}
