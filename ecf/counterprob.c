/* Problem 8.23 */

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
