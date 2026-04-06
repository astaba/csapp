/* =========================================================================
 * Created on: <Fri Mar 27 17:50:31 +01 2026> 
 * Time-stamp: <Fri Mar 27 17:52:55 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/forkprob4.c -
 * Practice Problem 8.12
 * How many “hello” output lines does this program print?
 * Answer: [[file:README.org::#problem-8-12]]
 * ========================================================================= */
#include "../include/csapp.h"

void doit() {
  Fork();
  Fork();
  printf("hello\n");
  return;
}

int main() {
  doit();
  printf("hello\n");
  exit(0);
}
