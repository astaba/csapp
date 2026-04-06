/* =========================================================================
 * Created on: <Wed Mar 25 14:39:44 +01 2026>
 * Time-stamp: <Thu Mar 26 15:54:42 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/youta/cgi-bin/yadder.c -
 * ========================================================================= */
#include "../ycsapp.h"
#include "html_template.c.inc"

int main(int argc, char *argv[argc + 1]) {
  char cgivar[13] = "QUERY_STRING";
  char *query, *p;
  char content[MAXBUF], arg1[MAXLINE], arg2[MAXLINE];
  int op1 = 0, op2 = 0;

  query = getenv(cgivar);
  if (query && (p = strchr(query, '&'))) {
    *p = '\0';
    strcpy(arg1, query);
    strcpy(arg2, p + 1);
    op1 = strtol(arg1, NULL, 0);
    op2 = strtol(arg2, NULL, 0);
  }

  /* Build body. */
  snprintf(content, MAXBUF, adder_template, op1, op2, op1 + op2);
  /* Build headers and send response. */
  printf("Connection: close\r\n");
  printf("Content-Type: text/html\r\n");
  printf("Content-Length: %zu\r\n", strlen(content));
  printf("\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(EXIT_SUCCESS);
}
