/* =========================================================================
 * Created on: <Sun Mar 15 11:10:20 +00 2026>
 * Time-stamp: <Sun Mar 15 11:39:00 +00 2026 by owner>
 * Author    : Astar Bahouidi
 * Desc      : /home/owner/coding/c_prog/csapp/netp/yechoclient.c
 * Practice sandbox for Figure 11.20 Echo client main routine.
 * ========================================================================= */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  int clientfd;
  char *host, *port, buf[MAXLINE];
  rio_t yio; /* Buffered IO State Monitor */

  if (argc != 3) {
    fprintf(stderr, "Usade: %s <host> <port>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }
  host = argv[1];
  port = argv[2];

  clientfd = Yopen_clientfd(host, port);
  Yio_readinitb(&yio, clientfd);

  while (Fgets(buf, MAXLINE, stdin) != NULL) {
    Yio_writen(clientfd, buf, strlen(buf));
    Yio_readlb(&yio, buf, MAXLINE);
    Fputs(buf, stdout);    
  }

  Close(clientfd);
  exit(EXIT_SUCCESS);
}
