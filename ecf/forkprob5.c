/* Practice Problem 8.14 */
/* How many “hello” output lines does this program print? */
/* $begin forkprob5 */
#include "csapp.h"

void doit() {
  if (Fork() == 0) {
    Fork();
    printf("hello\n");
    exit(0);
  }
  return;
}

int main() {
  doit();
  printf("hello\n");
  exit(0);
}
/* $end forkprob5 */

/* Practice Problem 8.14 */
/* How many “hello” output lines does this program print? */
/* $begin forkprob5 */
#include "csapp.h"

void doit() {
  if (Fork() == 0) {
    Fork();
    printf("hello\n");
    exit(0);
  }
  return;
}

int main() {
  doit();
  printf("hello\n");
  exit(0);
}
/* $end forkprob5 */
