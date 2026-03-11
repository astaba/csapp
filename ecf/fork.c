/* Figure 8.15 Using fork to create a new process. */

#include "../include/csapp.h"

int main() {
  pid_t pid;
  int x = 1;

  pid = Fork();
  if (pid == 0) { /* Child */
    printf("[PID:%d]child:\tx = %d\n", getpid(), ++x);
    exit(0);
  }

  /* Parent */
  printf("[PID:%d]parent:\tx = %d\n", getpid(), --x);
  exit(0);
}
