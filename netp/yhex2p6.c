/* =========================================================================
 * Created on: <Sat Apr 11 13:41:01 +01 2026> 
 * Time-stamp: <Mon Apr 20 15:26:55 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/yhex2p6.c -
 * Irrelevant conversion from 128-bit number to IPv6 hex-string.
 * The shinning purpose of this program is memory layout control during
 * integer vectorisation.
 * See [[file:README.org::#problem-11-2y]]
 * See [[file:~/coding/c_prog/tlpi/sockets/README.org::#inet-addrstrlen]]
 * ========================================================================= */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  char buf[INET6_ADDRSTRLEN];
  struct in6_addr addri6;
  uint32_t hostwords[4], inetword;
  size_t i;

  if (argc != 5 || !strcmp(argv[1], "--help")) {
    fprintf(stderr,
            "Usage: %s <(32-bit HEX) × 4>\n"
            "As an example:\n"
            "Enter this:\t0x20014860 0x48600000 0x0 0x8888\n"
            "Instead of:\t0x20014860486000000000000000008888\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  memset(&addri6, 0, sizeof(struct in6_addr));

  /* HACK: Better and Safer */
  for (i = 0; i < 4; i++) {
    hostwords[i] = (uint32_t)strtoul(argv[i + 1], NULL, 16);
    inetword = htonl(hostwords[i]);
    memcpy(addri6.s6_addr + (i * 4), &inetword, sizeof(uint32_t));
  }

  if (inet_ntop(AF_INET6, &addri6.s6_addr, buf, INET6_ADDRSTRLEN) == NULL)
    unix_error("inet_ntop() failed");

  printf("%s\n", buf);
  exit(0);
}
