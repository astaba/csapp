/* =========================================================================
 * Created on: <Fri Feb 13 12:23:19 +01 2026> 
 * Time-stamp: <Sat Mar 28 03:01:24 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/sigint.c -
 * Figure 8.30 A program that uses a signal handler to catch a
 * SIGINT signal.
 * WARN: Unsafe.  See [[file:sigintsafe.c]] for safe version.
 * ========================================================================= */
#include "../include/csapp.h"

void sigint_handler(int signal) {
  /* FIXME: Don't use this buffered IO call. */
  printf("\nCaught 'Ctrl C' signal of type: SIGINT(%d)\n", SIGINT);
  exit(0);
}

int main(void) {
  /* Install signal handler */
  if (signal(SIGINT, sigint_handler) == SIG_ERR)
    unix_error("signal() failed");

  pause();
}
