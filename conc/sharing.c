/* =========================================================================
 * Created on: <Wed Apr 22 22:59:58 +01 2026>
 * Time-stamp: <Fri May  1 15:33:35 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/sharing.c -
 *
 * Figure 12.15 Example program that illustrates different aspects of
 * sharing.
 * ========================================================================= */
#include "../include/csapp.h"
#define N 2

void *thread(void *vargp);

 /* Global variable: Unique to process; available to all threads. */
char **ptr;

int main() {
  int i;
  pthread_t tid;		/* Automatic variable: unique to main thread. */
  char *msgs[N] = {"Hello from foo", "Hello from bar"};

  ptr = msgs;
  for (i = 0; i < N; i++)
    Pthread_create(&tid, NULL, thread, (void *)i);
  Pthread_exit(NULL);
}

void *thread(void *vargp) {
  /* Local Automatic variable: each thread stack has its own */
  int myid = (int)vargp;
  /* Local static variable: unique to this scope call in every thread
     stack; declared once; locally modified with persistent golbal
     effetcs. */
  static int cnt = 0;
  printf("[%d]: %s (cnt=%d)\n", myid, ptr[myid], ++cnt); /* stack */
  return NULL;
}
