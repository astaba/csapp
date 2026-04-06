/* =========================================================================
 * Created on: <Fri Mar 27 19:57:06 +01 2026> 
 * Time-stamp: <Fri Mar 27 20:03:05 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob2.c -
 * Problem 8.18
 * Determine which of the following outputs are possible.
 * Answer: [[file:README.org::#problem-8-18]]
 * ========================================================================= */
#include "../include/csapp.h"

void end(void) {
  printf("2");
  fflush(stdout);
}

int main() {
  if (Fork() == 0)
    atexit(end);
  if (Fork() == 0) {
    printf("0");
    fflush(stdout);
  } else {
    printf("1");
    fflush(stdout);
  }
  exit(0);
}
