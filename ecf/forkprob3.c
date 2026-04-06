/* =========================================================================
 * Created on: <Fri Mar 27 19:12:39 +01 2026> 
 * Time-stamp: <Fri Mar 27 19:20:09 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob3.c -
 * Practice Problem 8.13
 * What is one possible output of the following program?
 * Answer: [[file:README.org::#problem-8-13]]
 * ========================================================================= */
#include "../include/csapp.h"

int main() {
  int x = 3;

  if (Fork() != 0)
    printf("x=%d\n", ++x);

  printf("x=%d\n", --x);
  exit(0);
}
