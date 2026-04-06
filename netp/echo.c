/* =========================================================================
 * Created on: <Mon Mar 30 22:38:37 +01 2026>
 * Time-stamp: <Mon Apr  6 21:04:16 +01 2026 by owner>
 * Author    : CS:APP
 * Desc      : ~/coding/c_prog/csapp/netp/echo.c -
 * Figure 11.22 read and echo text lines until client closes connection.
 * [[file:README.org::#purpose-of-echo-server]]
 * ========================================================================= */
#include "../include/csapp.h"

void echo(int connfd) {
  size_t n;
  char buf[MAXLINE];
  rio_t rio;

  Rio_readinitb(&rio, connfd);
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("server received %d bytes\n", (int)n);
    Rio_writen(connfd, buf, n);
  }
}
