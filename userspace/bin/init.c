// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *argv[] = { "sh", 0 };

int
main(void)
{
  int pid, wpid;

  /*
   * Create the console device.
   */
  int fd = open("console", O_RDWR);
  if(fd < 0){
    mknod("console", 1, 1);
    fd = open("console", O_RDWR);
  }

  if(fd < 0){
    printf( "init: cannot open console\n");
    exit();
  }

  dup(fd);  // stdout
  dup(fd);  // stderr

  for(;;){
    pid = fork();
    if(pid < 0){
      exit();
    }

    if(pid == 0){
      exec("/bin/sh", argv);
      printf("init: exec /bin/sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      ;

    /* Quick delay to prevent crash loops */
    sleep(1);
  }
}
