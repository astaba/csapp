/* =========================================================================
 * Created on: <Fri Mar 27 19:51:17 +01 2026> 
 * Time-stamp: <Fri Mar 27 19:56:19 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/waitprob1.c -
 * Practice Problem 8.4 (solution page 833)
 * Consider the following program:
 * A. How many output lines does this program generate?
 * B. What is one possible ordering of these output lines?
 * Problem 8.17: Enumerate all of the possible output of this program.
 * Answer: [[file:README.org::#problem-8-17]]
 * ========================================================================= */
#include "../include/csapp.h"

int main() {
  int status;
  pid_t pid;

  printf("Hello\n");
  pid = Fork();
  printf("%d\n", !pid);
  if (pid != 0) {
    if (waitpid(-1, &status, 0) > 0) {
      if (WIFEXITED(status) != 0)
        printf("%d\n", WEXITSTATUS(status));
    }
  }
  printf("Bye\n");
  exit(2);
}
