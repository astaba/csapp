/* Figure 8.29 Using the kill function to send a signal to a child. */
/* $begin kill */
#include "../include/csapp.h"

int main() {
  pid_t pid;

  /* Child until SIGKILL signal received, then dies */
  if ((pid = Fork()) == 0) {
    Pause(); /* Wait for a signal to arrive */
    printf("Control should never reach here!\n");
    exit(0);
  }

  /* Parent sends a SIGKILL signal to a child */
  Kill(pid, SIGKILL);
  exit(0);
}
