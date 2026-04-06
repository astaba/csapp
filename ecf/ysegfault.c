/* =========================================================================
 * Created on: <Fri Mar 27 16:58:45 +01 2026> 
 * Time-stamp: <Fri Mar 27 22:21:50 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/ysegfault.c -
 * Problem 8.24: Modify the program in
 * [[file:~/coding/c_prog/csapp/ecf/waitpid1.c]]
 * so that the following two conditions are met.
 * More details: [[file:README.org::#problem-8-24]]
 * ========================================================================= */
#include "../include/csapp.h"
#define N 2

/* Dummy function on the .text segment */
void foo() {};

int main() {
  size_t i;
  pid_t pid;
  int status;
  int *p = (int *)foo; /* Retreive address on the read-only .text segment */

  /* Parent create N children */
  for (i = 0; i < N; i++)
    if (Fork() == 0) {
      *p = 1; /* HACK: Overwrite on .text segment: trigger segfault */
      exit(100 + i); /* Control flow never gets here */
    }

  /* Parent reaps N children in no particular order */
  while ((pid = waitpid(-1, &status, 0)) > 0) {
    if (WIFEXITED(status)) {
      printf("[PID:%ld]child process: terminated normally: status = %d\n",
	     (long)pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      int sig = WTERMSIG(status);
      char s[MAXBUF];
      snprintf(s, MAXBUF, "child %ld terminated by signal %d", (long)pid, sig);
      psignal(sig, s);
    }
  }

  /* The only normal termination is if there are no more children */
  if (errno != ECHILD)
    unix_error("waitpid() failed");

  exit(0);
}
