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
    printf(2, "init: cannot open console\n");
    exit();
  }

  dup(fd);  // stdout
  dup(fd);  // stderr

  /*
   * Initially try to open initlog.txt as RDWR,
   * and if it fails create it.
   */
  int logfd = open("/etc/initlog.txt", O_RDWR);

  if(logfd < 0){
    logfd = open("/etc/initlog.txt", O_CREATE | O_RDWR);
    printf(logfd, "init: no /etc/initlog.txt, creating\n");
  }

  for(;;){
    printf(logfd, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(logfd, "init: fork failed\n");
      exit();
    }

    if(pid == 0){
      exec("/bin/sh", argv);
      printf(logfd, "init: exec /bin/sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(logfd, "init: reaped zombie %d\n", wpid);

    /* Quick delay to prevent crash loops */
    sleep(1);
  }
}
