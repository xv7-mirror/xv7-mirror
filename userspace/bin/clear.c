/*
 * clear.c - clear program for xv7
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
  /*
   * IOCTL (console, IOCTL_CLEAR_SCREEN, 0)
   */
  ioctl(1, 1, 0);

  exit();
}