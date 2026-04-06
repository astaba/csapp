/* =========================================================================
 * Created on: <Fri Mar 27 19:29:13 +01 2026> 
 * Time-stamp: <Fri Mar 27 19:39:08 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob6.c -
 * Practice Problem 8.15
 * How many “hello” output lines does this program print?
 * Answer: [[file:README.org::#problem-8-15]]
 * ========================================================================= */

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
