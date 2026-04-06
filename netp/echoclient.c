/* =========================================================================
 * Created on: <Mon Mar 30 22:27:04 +01 2026>
 * Time-stamp: <Mon Mar 30 22:32:49 +01 2026 by owner>
 * Author    : CS:APP
 * Desc      : ~/coding/c_prog/csapp/netp/echoclient.c -
 * Figure 11.20 Echo client main routine.
 * [[file:README.org::#purpose-of-echo-client]]
 * ========================================================================= */
#include "../include/csapp.h"

int main(int argc, char **argv) {
  int clientfd;
  char *host, *port, buf[MAXLINE];
  rio_t rio;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  host = argv[1];
  port = argv[2];

  clientfd = Open_clientfd(host, port);
  Rio_readinitb(&rio, clientfd);

  while (Fgets(buf, MAXLINE, stdin) != NULL) {
    Rio_writen(clientfd, buf, strlen(buf));
    Rio_readlineb(&rio, buf, MAXLINE);
    Fputs(buf, stdout);
  }
  Close(clientfd);
  exit(0);
}
