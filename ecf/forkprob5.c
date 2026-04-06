/* =========================================================================
 * CREATED on: <Fri Mar 27 19:17:43 +01 2026> 
 * Time-stamp: <Fri Mar 27 19:26:09 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob5.c -
 * Practice Problem 8.14
 * How many “hello” output lines does this program print?
 * Answer: [[file:README.org::#problem-8-14]]
 * ========================================================================= */
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
