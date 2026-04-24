/* =========================================================================
 * Created on: <Tue Apr 21 05:34:58 +01 2026>
 * Time-stamp: <Thu Apr 23 10:15:02 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/echoserverp.c -
 *
 * Figure 12.5 Concurrent echo server based on processes. The parent
 * forks a child to handle each new connection request.
 * See [[file:echoservers.c]], [[file:echoservert.c]]
 * ========================================================================= */
#include "../include/csapp.h"
void echo(int connfd);

void sigchld_handler(int sig) {
  while (waitpid(-1, 0, WNOHANG) > 0)
    ;
  return;
}

int main(int argc, char *argv[argc + 1]) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }

  Signal(SIGCHLD, sigchld_handler);
  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    if (Fork() == 0) {
      Close(listenfd); /* Child closes its listening socket */
      echo(connfd);    /* Child services client */
      Close(connfd);   /* Child closes connection with client */
      exit(0);         /* Child exits */
    }
    Close(connfd);
    /* Parent closes connected socket (important!) */
  }
}
