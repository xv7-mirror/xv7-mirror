/*
 * uptime.c - uptime program for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */

#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  printf("%d\n", uptime());

  return 0;
}