/* =========================================================================
 * Created on: <Fri Mar 27 19:37:13 +01 2026> 
 * Time-stamp: <Fri Mar 27 19:38:51 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob7.c -
 * Practice Problem 8.16
 * What is the output of the following program?
 * Answer: [[file:README.org::#problem-8-16]]
 * ========================================================================= */
#include "csapp.h"
int counter = 1;

int main() {
  if (fork() == 0) {
    counter--;
    exit(0);
  } else {
    Wait(NULL);
    printf("counter = %d\n", ++counter);
  }
  exit(0);
}
