/* =========================================================================
 * Created on: <Sat Apr 11 15:41:25 +01 2026>
 * Time-stamp: <Mon Apr 20 15:28:15 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/yp62hex.c -
 * See [[file:README.org::#problem-11-3y]]
 *
 * HACK: Since inetword endianess put bytes in human-counting order
 * You just take bytes from memory as they are and print. NOTE:
 * Pointer arithmethic: for each i index ─→ scale is 4. Because:
 * uint8_t s6_addr[16]
 * ========================================================================= */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  struct in6_addr addr;
  int rc;
  uint32_t inetword;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <IPv6 hex-string>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if ((rc = inet_pton(AF_INET6, argv[1], &addr.s6_addr)) != 1) {
    if (rc == 0)
      errno = EINVAL;
    unix_error("inet_pton() failed");
  };

  for (int i = 0; i < 4; i++) {
    memcpy(&inetword, addr.s6_addr + (i * 4), sizeof(uint32_t));
    printf("0x%08X ", ntohl(inetword));
  }

  puts("");
  exit(0);
}
