/* Created on: <Thu Mar 12 22:58:41 +00 2026 by owner> */
/* Time-stamp: <Fri Mar 13 00:20:36 +00 2026 by owner> */
/* Figure 11.17 Hostinfo displays the mapping of a domain name to its
   associated IP addresses. */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  struct addrinfo *addr, *listaddr, hints;
  int gai_err, gni_err;
  char buf[MAXBUF];

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <domain name>\n", argv[0]);
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
  addr = listaddr;
  do {
    gni_err = getnameinfo(addr->ai_addr, addr->ai_addrlen, buf, MAXBUF, NULL, 0,
                          NI_NUMERICHOST);
    if (gni_err) {
      fprintf(stderr, "getnameinfo() failed: %s", gai_strerror(gni_err));
      exit(EXIT_FAILURE);
    }
    printf("\t%s\n", buf);
  } while ((addr = addr->ai_next));

  freeaddrinfo(listaddr);
  exit(EXIT_SUCCESS);
}
