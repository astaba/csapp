/* =========================================================================
 * Created on: <Sun Mar 15 18:11:12 +00 2026> 
 * Time-stamp: <Sun Mar 15 23:21:09 +00 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/yechoserveri.c - 
 * ========================================================================= */
#include "../include/csapp.h"

void yecho(int conctfd);

int main(int argc, char *argv[argc + 1]) {
  char *port;
  int listenfd, conctfd;
  struct sockaddr_storage clientaddr;
  socklen_t clientlen;
  char client_host[MAXLINE], client_serv[MAXLINE];
  
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }
  port = argv[1];

  listenfd = Yopen_listenfd(port);
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    conctfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, client_host, MAXLINE, client_serv,
                MAXLINE, 0);
    printf("Connected to: (%s, %s)\n", client_host, client_serv);
    yecho(conctfd);
    Close(conctfd);
  }

  exit(EXIT_SUCCESS);
}
