/* =========================================================================
 * Created on: <Sat May 02 23:46:34 +01 2026>
 * Time-stamp: <Sun May  3 16:17:21 +01 2026 by owner>
 * Author    : Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/echoservert_cnt.c -
 *
 * A threads-based concurrent echo server that uses a thread-safe
 * version of echo() that counts the cumulative number of bytes
 * received by all threads from clients.
 * ========================================================================= */
#include "../include/csapp.h"

void echo_cnt(int connfd);
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

/* thread routine */
void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  Pthread_detach(pthread_self());
  Free(vargp);
  echo_cnt(connfd);
  Close(connfd);
  return NULL;
}
