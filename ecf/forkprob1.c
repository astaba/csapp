/* =========================================================================
 * Created on: <Fri Mar 27 17:43:49 +01 2026> 
 * Time-stamp: <Fri Mar 27 17:48:03 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob1.c -
 * Practice Problem 8.11
 * How many “hello” output lines does this program print?
 * Answer: [[file:README.org::#problem-8-11]]
 * ========================================================================= */
#include "../include/csapp.h"

int main() {
  int i;

  for (i = 0; i < 2; i++)
    Fork();
  printf("hello\n");
  exit(0);
}
