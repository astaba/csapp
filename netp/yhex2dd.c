/* =========================================================================
 * Created on: <Fri Apr 10 14:07:14 +01 2026>
 * Time-stamp: <Sat Apr 11 23:45:05 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/yhex2dd.c -
 * Practice Problem 11.2
 * Write a program that converts its 32-bit hex argument to a 32-bit
 * network byte order and print the result in dotted-decimal notation.
 * Reverse path [[file:ydd2hex.c]]
 * See [[file:README.org::#problem-11-2]]
 * See [[file:~/coding/c_prog/tlpi/sockets/README.org::#inet-addrstrlen]]
 * ========================================================================= */
#include "../include/csapp.h"
#include <netinet/in.h>

int main(int argc, char *argv[argc + 1]) {
  char buf[INET_ADDRSTRLEN];
  struct in_addr inaddr;
  uint32_t addr;

  if (argc != 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "Usage: %s <Hex number>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  /* hex string ─→ hexadecimal digit */
  addr = strtol(argv[1], NULL, 16);

  /* (Host LITTLE Endian → Network BIG Endian) = in_addr */
  inaddr.s_addr = htonl(addr);

  if (inet_ntop(AF_INET, &inaddr.s_addr, buf, INET_ADDRSTRLEN) == NULL)
    unix_error("inet_ntop() failed");

  printf("%s\n", buf);
}
