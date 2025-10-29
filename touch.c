/*
 * touch.c - touch program for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf(2, "Usage: touch files...\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    int fd = open(argv[i], O_CREATE | O_RDWR);

    if(fd < 0){
      printf(2, "touch: could not create %s\n", argv[i]);
    } else {
     close(fd);
    }
  }

  exit();
}

