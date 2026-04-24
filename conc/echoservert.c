/* =========================================================================
 * Created on: <Wed Apr 22 22:10:32 +01 2026>
 * Time-stamp: <Thu Apr 23 10:15:02 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/echoservert.c -
 *
 * Figure 12.14 Concurrent echo server based on threads.
 * See [[file:echoserverp.c]], [[file:echoservers.c]]
 * ========================================================================= */
#include "../include/csapp.h"

void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv) {
  int listenfd, *connfdp;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  pthread_t tid;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  listenfd = Open_listenfd(argv[1]);

  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfdp = Malloc(sizeof(int));
    *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Pthread_create(&tid, NULL, thread, connfdp);
  }
}

/* Thread routine */
void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  Pthread_detach(pthread_self());
  Free(vargp);
  echo(connfd);
  Close(connfd);
  return NULL;
}
