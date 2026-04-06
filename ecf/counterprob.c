/* =========================================================================
 * Created on: <Fri Mar 27 16:40:22 +01 2026>
 * Time-stamp: <Fri Mar 27 16:47:42 +01 2026 by owner>
 * Author    : CS:APP
 * Desc      : ~/coding/c_prog/csapp/ecf/counterprob.c -
 * Figure 8.45 Counter program referenced in Problem 8.23.
 * [[file:README.org::#problem-8-23]]
 * ========================================================================= */
#include "../include/csapp.h"

int counter = 0;

void sigint_handler(int sig) {
  counter++;
  sleep(1); /* Do some work in the handler */
  return;
}

int main() {
  int i;

  Signal(SIGUSR2, sigint_handler);

  if (Fork() == 0) { /* Child */
    for (i = 0; i < 5; i++) {
      Kill(getppid(), SIGUSR2);
      printf("sent SIGUSR2 N°%d to parent\n", i + 1);
    }
    exit(0);
  }

  Wait(NULL);
  printf("counter=%d\n", counter);
  exit(0);
}
