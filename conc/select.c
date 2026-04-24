/* =========================================================================
 * Created on: <Tue Apr 21 14:55:19 +01 2026>
 * Time-stamp: <Tue Apr 21 14:59:20 +01 2026 by owner>
 * Author    : CS:APP by Randal E. Bryant and David R. O’Hallaron
 * Desc      : ~/coding/c_prog/csapp/conc/select.c -
 *
 * Figure 12.6 An iterative echo server that uses I/O
 * multiplexing. The server uses select to wait for connection
 * requests on a listening descriptor and commands on standard input.
 * ========================================================================= */
#include "../include/csapp.h"

void echo(int connfd);
void command(void);

int main(int argc, char **argv) {
  int listenfd, connfd;
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  fd_set read_set, ready_set;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  listenfd = Open_listenfd(argv[1]);

  FD_ZERO(&read_set);              /* Clear read set */
  FD_SET(STDIN_FILENO, &read_set); /* Add stdin to read set */
  FD_SET(listenfd, &read_set);     /* Add listenfd to read set */

  while (1) {
    ready_set = read_set;
    Select(listenfd + 1, &ready_set, NULL, NULL, NULL);
    if (FD_ISSET(STDIN_FILENO, &ready_set))
      command(); /* Read command line from stdin */
    if (FD_ISSET(listenfd, &ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
      echo(connfd); /* Echo client input until EOF */
      Close(connfd);
    }
  }
}

void command(void) {
  char buf[MAXLINE];
  if (!Fgets(buf, MAXLINE, stdin))
    exit(0);         /* EOF */
  printf("%s", buf); /* Process the input command */
}
