/* =========================================================================
 * Created on: <Tue Apr 21 05:32:03 +01 2026>
 * Time-stamp: <Tue Apr 21 19:19:45 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/echo.c -
 *
 * Figure 11.22y read and echo text lines until client closes
 * connection. Modified to uppercase incoming client input before
 * echoing it back for the sake of visual feedback on the prompt.
 * ========================================================================= */
#include "../include/csapp.h"
#include <ctype.h>

void echo(int connfd) {
  ssize_t n;
  char buf[MAXLINE];
  rio_t rio;
  size_t j;

  Rio_readinitb(&rio, connfd);
  while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("server received %ld bytes\n", n);
    for (j = 0; (ssize_t)j < n; j++)
      buf[j] = toupper((unsigned char)buf[j]);
    Rio_writen(connfd, buf, n);
  }
}
