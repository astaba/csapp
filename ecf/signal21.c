/* =========================================================================
 * Created on: <Sun Mar 29 01:14:44 +01 2026>
 * Time-stamp: <Sun Mar 29 01:27:50 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/signal21.c -
 * Server grade improvement of [[file:signal2.c][Figure 8.37 signal2]].
 * See [[file:README.org::#sigchld-for-production-grade-server]]
 * ========================================================================= */
#include "../include/csapp.h"

void handler2(int sig) {
  int olderrno = errno;
  pid_t pid;

  /* NOTE: Unblock with WNOHANG and return 0 when no child is ready to be
   * reaped. */
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    Sio_puts("Handler reaped child\n");
  }
  /* WARN: Only ever check errno when an actual error ocurred. */
  /* Try removing the "pid < 0 &&" test to see why. */
  if (pid < 0 && errno != ECHILD)
    Sio_error("waitpid error");
  Sleep(1); /* Models shell various cleanup after reaping child process. */
  errno = olderrno;
}

int main() {
  int i, n;
  char buf[MAXBUF];

  if (signal(SIGCHLD, handler2) == SIG_ERR)
    unix_error("signal error");

  /* Parent creates children */
  for (i = 0; i < 3; i++) {
    if (Fork() == 0) {
      printf("Hello from child %d\n", (int)getpid());
      exit(0);
    }
  }
  /* Model shell prompt: Parent waits for terminal input and then processes it.
   * The infinite loop models the command line processing after its input on a
   * terminal or server. */
  if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0)
    unix_error("read");
  printf("Parent processing input\n");
  while (1)
    ;

  exit(0);
}
