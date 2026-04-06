/* Figure 8.36 signal1.
 * WARN: This program is flawed because it assumes that signals
 * are queued. See [[file:signal2.c]]  */
#include "../include/csapp.h"

void handler1(int sig) {
  int olderrno = errno;

  if ((waitpid(-1, NULL, 0)) < 0)
    sio_error("waitpid error");
  Sio_puts("Handler reaped child\n");
  Sleep(1); /* Models shell various cleanup after reaping child process. */
  errno = olderrno;
}

int main() {
  int i, n;
  char buf[MAXBUF];

  if (signal(SIGCHLD, handler1) == SIG_ERR)
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
