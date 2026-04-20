/* =========================================================================
 * Created on: <Fri Apr 10 14:50:05 +01 2026>
 * Time-stamp: <Sat Apr 11 23:38:29 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/ydd2hex.c -
 * Practice Problem 11.3
 * Write a program that takes a dotted-decimal notation string and
 * convert it to hexadecimal IP Address.
 * Reverse path [[file:yhex2dd.c]]
 * See [[file:README.org::#problem-11-3]]
 * ========================================================================= */
#include "../include/csapp.h"

int main(int argc, char *argv[argc + 1]) {
  struct in_addr addri4;
  int rc;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <0.0.0.0>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Dotted Decimal String → BIG Endian uint32_t */
  if ((rc = inet_pton(AF_INET, argv[1], &addri4.s_addr)) != 1) {
    if (rc == 0)
      fprintf(stderr, "ERROR: Bad dotted decimal: '%s'\n", argv[1]);
    else
      unix_error("inet_pton() failed");
    exit(EXIT_FAILURE);
  }

  /* (Network BIG Endian → Host LITTLE Endian) then printf */
  printf("%#x\n", ntohl(addri4.s_addr));

  exit(EXIT_SUCCESS);
}
