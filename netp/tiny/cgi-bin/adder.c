/* =========================================================================
 * Created on: <Sat Mar 21 02:20:51 +00 2026>
 * Time-stamp: <Mon Apr  6 17:16:00 +01 2026 by owner>
 * Author    : Astar Bahouidi
 * Desc      : ~/coding/c_prog/csapp/netp/tiny/cgi-bin/adder.c -
 * Figure 11.27 CGI program that sums two integers.
 * a minimal CGI program that adds two numbers together
 * [[file:~/coding/c_prog/csapp/netp/README.org::#problem-11-5]]
 * [[file:~/coding/c_prog/csapp/netp/README.org::#problem-11-10]]
 * ========================================================================= */
#include "../csapp.h"
#include <cjson/cJSON.h>

static void parseUrlParams(char *params, int *opr1, int *opr2);
static void decodeUrl(char *dest, const char *src);

int main(void) {
  char *urlEncoded = NULL, *method, *content_lstr, *content_type, *jsonStr;
  char *jsonPretty; /* DEBUG: */
  char content_buf[MAXLINE], urlParams[MAXLINE];
  int n1 = 0, n2 = 0, content_lnum = 0;
  int is_GET = 0, is_POST = 0, is_HEAD = 0;

  method = getenv("REQUEST_METHOD"); /* INFO: Problem 11.11 */
  if (method) {
    is_GET = !strcasecmp(method, "GET");
    is_POST = !strcasecmp(method, "POST");
    is_HEAD = !strcasecmp(method, "HEAD");
  }

  if (is_GET) {
    urlEncoded = getenv("QUERY_STRING");
  } else if (is_POST) {
    content_type = getenv("CONTENT_TYPE");
    if ((content_lstr = getenv("CONTENT_LENGTH")) == NULL) {
      fprintf(stderr, "ERROR: Missing CONTENT_LENGTH\n");
      exit(1);
    };
    content_lnum = strtol(content_lstr, NULL, 10);

    if (content_lnum > MAXLINE) {
      fprintf(stderr, "ERROR: Payload is too large: %d\n", content_lnum);
      /* FIXME: In a production server this flow-branch must be handled. */
      exit(1);
    }

    size_t nread = fread(content_buf, 1, content_lnum, stdin);
    /* NOTE: Handle fread error */
    if (nread != (size_t)content_lnum) {
      fprintf(stderr, "fread(): Partial read (%zu/%d) or failure: %s\n", nread,
              content_lnum, strerror(errno));
    }
    content_buf[content_lnum] = '\0';
    fprintf(stderr, "DEBUG: ADDER: Content-Type\n'%s'\n", content_type);
    /* Unify GET and POST path. */
    if (content_type && strcasestr(content_type, "x-www-form-urlencoded"))
      urlEncoded = content_buf;
    /* TODO: handle negative conditional */
  }

  if (urlEncoded) {
    fprintf(stderr, "DEBUG: ADDER: Request Content\n%s\n\n", urlEncoded);
    /* Decode and parse client data */
    decodeUrl(urlParams, urlEncoded);
    parseUrlParams(urlParams, &n1, &n2);
  }

  /* Build JSON reponse payload */
  cJSON *root = cJSON_CreateObject();
  cJSON_AddStringToObject(root, "status", "success");
  cJSON *data = cJSON_CreateObject();
  cJSON_AddNumberToObject(data, "operand1", n1);
  cJSON_AddNumberToObject(data, "operand2", n2);
  cJSON_AddNumberToObject(data, "result", n1 + n2);
  cJSON_AddItemToObject(root, "data", data);
  /* WARN: Both cJSON stringuifying functions allocate dynamic memory! */
  jsonStr = cJSON_PrintUnformatted(root);
  jsonPretty = cJSON_Print(root); /* DEBUG: */
  fprintf(stderr, "DEBUG: ADDER: Response payload\n%s\n", jsonPretty);
  free(jsonPretty); /* DEBUG: */

  /* Generate the HTTP response */
  printf("Content-Length: %d\r\n", (int)strlen(jsonStr));
  printf("Content-Type: application/json\r\n");
  printf("\r\n");

  if (!is_HEAD) { /* INFO: Problem 11.11: Send response body */
    printf("%s", jsonStr);
  }

  /* Clean up */
  fflush(stdout);
  free(jsonStr);
  cJSON_Delete(root);
  exit(0);
}

static void parseUrlParams(char *params, int *opr1, int *opr2) {
  char *token, *saveptr, *p;
  if (!params || *params == '\0')
    return;

  token = strtok_r(params, "&", &saveptr);
  while (token) {
    if ((p = strchr(token, '='))) {
      *p = '\0';
      if (!strcmp(token, "a"))
        *opr1 = strtol(p + 1, NULL, 10);
      if (!strcmp(token, "b"))
        *opr2 = strtol(p + 1, NULL, 10);
    }
    token = strtok_r(NULL, "&", &saveptr);
  }
}

static void decodeUrl(char *dest, const char *src) {
  char hex[3] = {0}; /* Null terminate */

  if (src) {
    for (; *src; ++src, ++dest) {
      if (*src == '+') {
        *dest = ' ';
      } else if (src[0] == '%' && isxdigit(src[1]) && isxdigit(src[2])) {
        hex[0] = src[1];
        hex[1] = src[2];
        *dest = (char)strtol(hex, NULL, 16);
        src += 2;
      } else {
        *dest = *src;
      }
    }
  }
  *dest = '\0';
}
