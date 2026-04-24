/* =========================================================================
 * Created on: <Wed Apr 22 21:51:12 +01 2026>
 * Time-stamp: <Wed Apr 22 21:54:22 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/hello.c -
 *
 * Figure 12.13 hello.c: The Pthreads “Hello, world!” program.
 * ========================================================================= */
#include "../include/csapp.h"

void *thread(void *vargp);

int main() {
  pthread_t tid;

  Pthread_create(&tid, NULL, thread, NULL);
  Pthread_join(tid, NULL);

  exit(0);
}

/* thread routine */
void *thread(void *vargp) {
  printf("Hello, world!\n");
  return NULL;
}
