/* =========================================================================
 * Created on: <Thu Apr 23 00:08:46 +01 2026>
 * Time-stamp: <Fri Apr 24 00:56:57 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/badcnt.c -
 *
 * Figure 12.16 badcnt.c: An improperly synchronized counter program.
 * WARN: This code is buggy!
 * ========================================================================= */
#include "../include/csapp.h"

void *thread(void *vargp); /* Thread routine prototype */

/* Global shared variable */
volatile long cnt = 0; /* Counter */

int main(int argc, char **argv) {
  long niters; /* [n]umber of [iter]ation[s] */
  pthread_t tid1, tid2;		/* [t]hread [id]s */

  /* Check input argument */
  if (argc != 2) {
    printf("usage: %s <niters>\n", argv[0]);
    exit(0);
  }
  niters = atoi(argv[1]);

  /* Create threads and wait for them to finish */
  Pthread_create(&tid1, NULL, thread, &niters);
  Pthread_create(&tid2, NULL, thread, &niters);
  Pthread_join(tid1, NULL);
  Pthread_join(tid2, NULL);

  /* Check result */
  if (cnt != (2 * niters))
    printf("BOOM! cnt=%ld\n", cnt);
  else
    printf("OK cnt=%ld\n", cnt);
  exit(0);
}

/* Thread routine: [[file:badcnt.asm::thread: ; thread routine]] */
void *thread(void *vargp) {
  long i, niters = *((long *)vargp);
  /* [[file:badcnt.asm::.L8: ; Body of each For loop iteration]] */
  for (i = 0; i < niters; i++)
    cnt++;

  return NULL;
}
