/* =========================================================================
 * Created on: <Fri Mar 27 21:28:52 +01 2026> 
 * Time-stamp: <Fri Mar 27 21:32:51 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob8.c -
 * Problem 8.19
 * How many lines of output does the following function print if the
 * value of n entered by the user is 6?
 * Answer: [[file:README.org::#problem-8-19]]
 * ========================================================================= */
#include "csapp.h"

void foo(int n) {
  int i;

  for (i = 0; i < n; i++)
    Fork();
  printf("hello\n");
  exit(0);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("usage: %s <n>\n", argv[0]);
    exit(0);
  }
  foo(atoi(argv[1]));
  exit(0);
}
