/* Figure 8.39 A shell program.
 * WARN: with a subtle synchronization error.
 * If the child terminates before the parent is able to run,
 * then addjob and deletejob will be called in the wrong order.
 * See [[file:procmask2.c]] for improvement. */
#include "../include/csapp.h"

void initjobs() {}
void addjob(int pid) {}
void deletejob(int pid) {}

/* $begin procmask1 */
/* WARNING: This code is buggy! */
void handler(int sig) {
  int olderrno = errno;
  sigset_t mask_all, prev_all;
  pid_t pid;

  Sigfillset(&mask_all);
  while ((pid = waitpid(-1, NULL, 0)) > 0) { /* Reap a zombie child */
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    deletejob(pid); /* Delete the child from the job list */
    Sigprocmask(SIG_SETMASK, &prev_all, NULL);
  }
  if (errno != ECHILD)
    Sio_error("waitpid error");
  errno = olderrno;
}

int main(int argc, char **argv) {
  int pid;
  sigset_t mask_all, prev_all;

  Sigfillset(&mask_all);    /* Keep mask_all with all signals */
  Signal(SIGCHLD, handler); /* Install SIGCHLD handler */
  initjobs();               /* Initialize the job list */

  while (1) {
    if ((pid = Fork()) == 0) { /* Child process */
      Execve("/bin/date", argv, NULL);
    }
    Sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* Parent process */
    addjob(pid); /* Add the child to the job list */
    Sigprocmask(SIG_SETMASK, &prev_all, NULL);
  }
  exit(0);
}
/* $end procmask1 */
