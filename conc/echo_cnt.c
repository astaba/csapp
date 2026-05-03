/* =========================================================================
 * Created on: <Sat May 02 23:51:05 +01 2026>
 * Time-stamp: <Sat May  2 23:53:46 +01 2026 by owner>
 * Author    : Copyright (c) 2002, R. Bryant and D. O'Hallaron,
 *	       All rights reserved.
 * Desc      : ~/coding/c_prog/csapp/conc/echo_cnt.c -
 *
 * Figure 12.29: A thread-safe version of echo that counts the total
 * number of bytes received from clients.
 * ========================================================================= */
#include "../include/csapp.h"

static int byte_cnt; /* Byte counter */
static sem_t mutex;  /* and the mutex that protects it */

static void init_echo_cnt(void) {
  Sem_init(&mutex, 0, 1);
  byte_cnt = 0;
}

void echo_cnt(int connfd) {
  int n;
  char buf[MAXLINE];
  rio_t rio;
  static pthread_once_t once = PTHREAD_ONCE_INIT;

  Pthread_once(&once, init_echo_cnt);
  Rio_readinitb(&rio, connfd);
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    P(&mutex);
    byte_cnt += n;
    printf("server received %d (%d total) bytes on fd %d\n", n, byte_cnt,
           connfd);
    V(&mutex);
    Rio_writen(connfd, buf, n);
  }
}
