// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "stat.h"
#include "types.h"
#include "user.h"

#define N 1000

int forktest(void) {
  int n, pid;

  printf("fork test\n");

  for (n = 0; n < N; n++) {
    pid = fork();
    if (pid < 0)
      break;
    if (pid == 0)
      return 0;
  }

  if (n == N) {
    printf("fork claimed to work N times!\n", N);
    return 0;
  }

  for (; n > 0; n--) {
    if (wait() < 0) {
      printf("wait stopped early\n");
      return 0;
    }
  }

  if (wait() != -1) {
    printf("wait got too many\n");
    return 0;
  }

  printf("fork test OK\n");
}

int main(void) {
  forktest();
  return 0;
}
