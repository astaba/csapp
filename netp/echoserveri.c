/* =========================================================================
 * Created on: <Mon Mar 30 22:34:45 +01 2026>
 * Time-stamp: <Mon Mar 30 22:43:54 +01 2026 by owner>
 * Author    : CS:APP
 * Desc      : ~/coding/c_prog/csapp/netp/echoserveri.c -
 * Figure 11.21 Iterative echo server main routine.
 * [[file:README.org::*Purpose of Echo Server and its Echo IO-Loop]]
 * ========================================================================= */
#include "../include/csapp.h"

void echo(int connfd);

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr; /* Enough space for any address */
  char client_hostname[MAXLINE], client_port[MAXLINE];

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
                client_port, MAXLINE, 0);
    printf("Connected to (%s, %s)\n", client_hostname, client_port);
    echo(connfd);
    Close(connfd);
  }
  exit(0);
}
