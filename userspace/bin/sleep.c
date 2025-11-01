/*
 * sleep.c - sleep program for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */

#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int ticks;
  if(argc < 2){
    printf( "Usage: sleep ticks\n");
    exit();
  }

  ticks = atoi(argv[1]);  /* Convert argv[1] to an integer */

  if(ticks <= 0)
  {
    /*
     * atoi is known to get borked if the number is
     * too large.
     */
    printf( "invalid number of ticks: %d\n", ticks);
    printf( "atoi() gets borked on too large numbers so pick a better one\n");
    exit();
  }

  if(ticks > 10000)
  {
    /* We don't want the user borking their system */
    printf( "this might be a bit too much\n");
    exit();
  }

  sleep(ticks);
  printf( "slept %d ticks\n", ticks);

  exit();
}