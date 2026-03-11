/* Figure 8.30 A program that uses a signal handler to catch a SIGINT
   signal. */
/* Created on: Fri Feb 13 12:23:19 +01 2026 */
#include "../include/csapp.h"

void sigint_handler(int signal) {
  printf("\nCaught 'Ctrl C' signal of type: SIGINT(%d)\n", SIGINT);
  exit(0);
}

int main(void) {
  /* Install signal handler */
  if (signal(SIGINT, sigint_handler) == SIG_ERR)
    unix_error("signal() failed");

  pause();
}
