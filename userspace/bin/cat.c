#include "stat.h"
#include "types.h"
#include "user.h"

char buf[512];

int cat(int fd) {
  int n;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    if (write(1, buf, n) != n) {
      printf("cat: write error\n");
      return 0;
    }
  }
  if (n < 0) {
    printf("cat: read error\n");
    return 0;
  }
}

int main(int argc, char *argv[]) {
  int fd, i;

  if (argc <= 1) {
    cat(0);
    return 0;
  }

  for (i = 1; i < argc; i++) {
    if ((fd = open(argv[i], 0)) < 0) {
      printf("cat: cannot open %s\n", argv[i]);
      return 0;
    }
    cat(fd);
    close(fd);
  }
  return 0;
}
