/* Problem 8.25 */
#include "../include/csapp.h"

/* Global jump buffer to allow the signal handler to return back to
   tfgets context. */
sigjmp_buf env;

/* Signal handler for SIGALRM. It jumps back to tfgets context and
   makes sigsetjmp return 1. */
static void handler(int sig) { siglongjmp(env, 1); }

/* A version of fgets that times out after SECS seconds. Return: S on
 success, NULL on timeout or error. */
char *tfgets(char *s, int n, FILE *stream, int secs) {
  if (Signal(SIGALRM, handler) == SIG_ERR) { /* Install handler */
    sio_puts(strerror(errno));
    return NULL;
  }
  /* sigsetjmp saves the current stack context. It returns 0 on first
     call. Afterward, it is expected to return the second argument of
     siglongjmp */
  if (sigsetjmp(env, 1) != 0) {
    /* At this point timer expired and handler called siglongjmp */
    alarm(0); /* Cancel any remaining alarm just in case. */
    return NULL;
  }

  alarm(secs); /* Start SECS countdown */

  char *result = fgets(s, n, stream); /* Block on fgets */
  /* fgets returned before alarm went off: Cancel alaram immediately. */
  alarm(0);
  /* Trim terminating newline char */
  result[strcspn(s, "\n")] = '\0';
  return result;
}

int main(int argc, char *argv[argc + 1]) {
  if (argc < 2) {
    fprintf(stderr, "ERROR Usage: %s <secs>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int t = atoi(argv[1]);

  char buf[MAXBUF];
  printf("You have %d seconds to type something:\n", t);

  char *input = tfgets(buf, MAXBUF, stdin, t);
  if (input == NULL) {
    printf("\n[Time out!] No input received.\n");
  } else {
    printf("You input: '%s'\n", input);
  }

  exit(EXIT_SUCCESS);
}
