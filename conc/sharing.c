/* =========================================================================
 * Created on: <Wed Apr 22 22:59:58 +01 2026>
 * Time-stamp: <Wed Apr 22 23:06:15 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/sharing.c -
 *
 * Figure 12.15 Example program that illustrates different aspects of
 * sharing.
 * ========================================================================= */
#include "../include/csapp.h"
#define N 2

void *thread(void *vargp);

char **ptr; /* Global variable */

int main() {
  int i;
  pthread_t tid;
  char *msgs[N] = {"Hello from foo", "Hello from bar"};

  ptr = msgs;
  for (i = 0; i < N; i++)
    Pthread_create(&tid, NULL, thread, (void *)i);
  Pthread_exit(NULL);
}

void *thread(void *vargp) {
  int myid = (int)vargp;
  static int cnt = 0;
  printf("[%d]: %s (cnt=%d)\n", myid, ptr[myid], ++cnt); /* stack */
  return NULL;
}
