/* =========================================================================
 * Created on: <Fri Apr 24 00:57:07 +01 2026>
 * Time-stamp: <Fri May  1 16:42:26 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/goodcnt.c -
 *
 * A correctly synchronized counter program
 * ========================================================================= */
#include "../include/csapp.h"

void *thread(void *vargp); /* Thread routine prototype */

/* Global shared variables */
static volatile long cnt = 0; /* Counter */
/* Declare Unamed Semaphore in shared memory region to protect counter */
static sem_t mutex;

int main(int argc, char **argv) {
  int niters;
  pthread_t tid1, tid2;

  /* Check input argument */
  if (argc != 2) {
    printf("usage: %s <niters>\n", argv[0]);
    exit(0);
  }
  niters = atoi(argv[1]);

  /* arg2 = 0 # Semaphore is not process-shared */
  /* arg3 = 1 # Semaphore initial value */
  Sem_init(&mutex, 0, 1);

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

/* Thread routine */
void *thread(void *vargp) {
  int i, niters = *((int *)vargp);

  for (i = 0; i < niters; i++) {
    P(&mutex); /* sem_wait(&mutex) */
    cnt++;
    V(&mutex); /* sem_post(&mutex) */
  }
  return NULL;
}
