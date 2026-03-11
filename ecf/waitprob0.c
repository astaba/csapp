/* Practice Problem 8.3 (solution page 833) */
/* List all of the possible output sequences for the following program: */

#include "../include/csapp.h"

int main() {
  pid_t rc;
  rc = Fork();
  if (rc == 0) {
    printf("[PID:%ld] 'a'\n", (long)getpid());
    fflush(stdout);
  } else {
    printf("[PID:%ld] 'b'\n", (long)getpid());
    fflush(stdout);
    waitpid(-1, NULL, 0);
  }
  printf("[PID:%ld] 'c'\n", (long)getpid());
  fflush(stdout);
  exit(0);
}
