/* =========================================================================
 * Created on: <Fri Mar 27 21:48:33 +01 2026> 
 * Time-stamp: <Fri Mar 27 21:51:42 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/waitprob3.c -
 * Problem 8.21
 * What are the possible output sequences from the following program?
 * Answer: [[file:README.org::#problem-8-21]]
 * ========================================================================= */
#include "../include/csapp.h" 	/* IWYU pragma: keep */

int main() {
  if (fork() == 0) {
    printf("a");
    fflush(stdout);
    exit(0);
  } else {
    printf("b");
    fflush(stdout);
    waitpid(-1, NULL, 0);
  }
  printf("c");
  fflush(stdout);
  exit(0);
}
