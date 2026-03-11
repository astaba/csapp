/* Figure 8.19 Using waitpid to reap zombie children in the order they
   were created. */

#include "../include/csapp.h"
#define N 5

int main() {
  size_t i;
  int status;
  pid_t pids[N], retpid;

  /* Parent create N children */
  for (i = 0; i < N; i++)
    if ((pids[i] = Fork()) == 0)
      exit(100 + i);

  /* Parent reaps N children in order */
  i = 0;
  while ((retpid = waitpid(pids[i++], &status, 0)) > 0) {
    if (WIFEXITED(status)) {
      printf("child pid=%ld terminated normally: status=%d\n", (long)retpid,
             WEXITSTATUS(status));
    } else {
      printf("child pid=%ld terminated abnormally\n", (long)retpid);
    }
  }

  /* The only normal termination is for no more children to wait for. */
  if (errno != ECHILD)
    unix_error("waitpid() failed");

  exit(0);
}
