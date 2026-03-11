/* Figure 8.37 signal2. An improved version of Figure 8.36 that correctly
 * accounts for the fact that signals are not queued. */

#include "../include/csapp.h"

/* $begin signal2 */
void handler2(int sig) {
  int olderrno = errno;

  while (waitpid(-1, NULL, 0) > 0) {
    Sio_puts("Handler reaped child\n");
  }
  if (errno != ECHILD)
    Sio_error("waitpid error");
  Sleep(1); /* Models shell various cleanup after reaping child process. */
  errno = olderrno;
}
/* $end signal2 */

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
