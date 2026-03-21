/* =========================================================================
 * Created on: <Sun Mar 15 22:31:26 +00 2026>
 * Time-stamp: <Sun Mar 15 22:42:31 +00 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/yecho.c -
 * Sandbox for Figure 11.22 echo function that reads and echoes text lines.
 * ========================================================================= */
#include "../include/csapp.h"

void yecho(int conctfd) {
  size_t n;
  rio_t yio;
  char buf[MAXLINE];

  Yio_readinitb(&yio, conctfd);
  while ((n = Yio_readlb(&yio, buf, MAXLINE)) != 0) {
    printf("server received: %zu bytes\n", n);
    Yio_writen(conctfd, buf, n);
  }
}
