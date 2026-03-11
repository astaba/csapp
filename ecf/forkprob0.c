/* Practice problem 8.2 (solution page 831) */
/* Consider the following program: */
/* A. What is the output of the child process? */
/* B. What is the output of the parent process? */

#include "../include/csapp.h"

/* $begin forkprob0 */
int main() {
  int x = 1;

  if (Fork() == 0)
    printf("p1: x=%d\n", ++x); // line:ecf:forkprob0:childfork
  printf("p2: x=%d\n", --x);   // line:ecf:forkprob0:parentfork
  exit(0);
}
/* $end forkprob0 */
