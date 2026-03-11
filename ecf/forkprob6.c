/* Practice Problem 8.15 */
/* How many “hello” output lines does this program print? */
/* $begin forkprob6 */
#include "csapp.h"

void doit() {
  if (Fork() == 0) {
    Fork();
    printf("hello\n");
    return;
  }
  return;
}

int main() {
  doit();
  printf("hello\n");
  exit(0);
}
/* $end forkprob6 */

/* Practice Problem 8.16 */
/* What is the output of the following program? */
/* $begin forkprob7 */
#include "csapp.h"
int counter = 1;

int main() {
  if (() != 0)
    printf("x=%d\n", ++x);

  printf("x=%d\n", --x);
  exit(0);
}
/* $end forkprob3 */

/* Practice Problem 8.15 */
/* How many “hello” output lines does this program print? */
/* $begin forkprob6 */
#include "csapp.h"

void doit() {
  if (Fork() == 0) {
    Fork();
    printf("hello\n");
    return;
  }
  return;
}

int main() {
  doit();
  printf("hello\n");
  exit(0);
}
/* $end forkprob6 */

/* Practice Problem 8.16 */
/* What is the output of the following program? */
/* $begin forkprob7 */
#include "csapp.h"
int counter = 1;

int main() {
  if (Fork() == 0) {
    counter--;
    exit(0);
  } else {
    Wait(NULL);
    printf("counter = %d\n", ++counter);
  }
  exit(0);
}
/* $end forkprob7 */
