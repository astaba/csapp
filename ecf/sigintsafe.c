/* =========================================================================
 * Created on: <Sat Mar 28 02:51:31 +01 2026>
 * Time-stamp: <Sat Mar 28 02:58:32 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/sigintsafe.c -
 * Figure 8.35 A safe version of SIGINT handler from [[file:sigint.c]]
 * ========================================================================= */
#include "../include/csapp.h"

/* Safe SIGINT handler */
void sigint_handler(int sig) {
  Sio_puts("Caught SIGINT!\n"); /* Safe output */
  _exit(0);                     /* Safe exit */
}

int main() {
  /* Install the SIGINT handler */
  if (signal(SIGINT, sigint_handler) == SIG_ERR)
    unix_error("signal error");

  pause(); /* Wait for the receipt of a signal */

  return 0;
}
