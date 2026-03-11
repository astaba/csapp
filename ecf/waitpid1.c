/* Figure 8.18 Using the waitpid function to reap zombie children in
   no particular order. */

#include "../include/csapp.h"
#define N 2

int main() {
  size_t i;
  pid_t pid;
  int status;

  /* Parent create N children */
  for (i = 0; i < N; i++)
    if (Fork() == 0)
      exit(100 + i);

  /* Parent reaps N children in no particular order */
  while ((pid = waitpid(-1, &status, 0)) > 0) {
    if (WIFEXITED(status)) {
      printf("[PID:%ld]child process: terminated normally: status = %d\n",
             (long)pid, WEXITSTATUS(status));
    } else {
      printf("[PID:%ld]child process: exited abnormally\n", (long)pid);
    }
  }

  /* The only normal termination is if there are no more children */
  if (errno != ECHILD)
    unix_error("waitpid() failed");

  exit(0);
}
