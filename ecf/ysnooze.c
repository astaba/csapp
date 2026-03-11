/* Practice Problem 8.7 (solution page 834) */
/* See README.org */

#include "../include/csapp.h"

static unsigned int snooze(unsigned int secs) {
  unsigned int rc = sleep(secs);
  printf("Slept for %d of %d\n", secs - rc, secs);
  return rc;
}

static void handler(int sig) { return; }

int main(int argc, char *argv[argc + 1]) {
  if (argc < 2 || strcmp(argv[1], "-h") == 0) {
    fprintf(stderr, "Usage: %s time_secs\n", argv[0]);
    exit(1);
  }

  if (signal(SIGINT, handler) == SIG_ERR) /* Install SIGINT handler */
    unix_error("signal() failed");

  (void)snooze(atoi(argv[1]));

  return EXIT_SUCCESS;
}
