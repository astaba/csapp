/* Created on: <Thu Mar 12 23:08:18 +00 2026 by owner> */
/* Time-stamp: <Fri Mar 13 00:15:15 +00 2026 by owner> */
/* Practice Problem 11.4 (solution page 1004) */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  struct addrinfo *addr, *listaddr, hints;
  struct sockaddr_in *sockp;
  int gai_err;
  char buf[MAXLINE];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  gai_err = getaddrinfo(argv[1], NULL, &hints, &listaddr);
  if (gai_err) {
    fprintf(stderr, "getaddrinfo() failed: %s", gai_strerror(gai_err));
    exit(EXIT_FAILURE);
  }

  printf("Hostname resolved to remote address(es):\n");
  for (addr = listaddr; addr; addr = addr->ai_next) {
    sockp = (struct sockaddr_in *)addr->ai_addr;
    if (inet_ntop(AF_INET, &(sockp->sin_addr), buf, MAXLINE) == NULL) {
      fprintf(stderr, "inet_ntop() failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("\t%s\n", buf);
  }

  freeaddrinfo(listaddr);
  exit(EXIT_SUCCESS);
}
