/* =========================================================================
 * Created on: <Sat Apr 04 13:39:17 +01 2026>
 * Time-stamp: <Mon Apr  6 01:28:22 +01 2026 by owner>
 * Author    : Astar Bahouidi
 * Desc      : ~/coding/c_prog/csapp/netp/tiny/tiny12.c -
 *             tiny.c - A simple, iterative HTTP/1.0 Web server that uses
 *             the GET method to serve static and dynamic content.
 *
 * Problem 11.12 Extend Tiny so that it serves dynamic content requested
 * by the HTTP POST method. Check your work using your favoriteWeb browser.
 * ========================================================================= */
#include "../../.env.c.inc"
#include "csapp.h"
#include "public/error_template.c.inc"

#define MINSTR 16

/* Aggregated HTTP request state.
 * Populated once in parse_request() and passed down the pipeline.
 * Acts as the single source of truth for routing, headers, and body.
 */
typedef struct {
  char method[MINSTR];
  char uri[MAXLINE];
  char version[MINSTR];
  int content_length;
  char content_type[MAXLINE];
  char body[MAXLINE];
  char filename[MAXLINE];
  char cgiargs[MAXLINE];
} httpreq_t;

static void doit(int fd);
static int parse_request(rio_t *rp, httpreq_t *req);
static int parse_uri(httpreq_t *req);
static void serve_static(int fd, int filesize, httpreq_t *req);
static void serve_dynamic(int fd, httpreq_t *req);
static void get_filetype(char *filename, char *filetype);
static void clienterror(int fd, char *cause, int errnum, char *explain);
static int supportedMethod(const char *method); /* INFO: Problem 11.11 part 2 */

/**
 * sigchld_handler - Asynchronous Zombie Reaper.
 * Logic: Problem 11.8. Uses WNOHANG to non-blockingly reap all terminated
 * children in the process group, preventing process table exhaustion.
 */
static void sigchld_handler(int sig) { /* INFO: Problem 11.8 part 1 start */
  int old_errno = errno;
  pid_t pid;

  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    ;
  if ((pid < 0) && (errno != ECHILD))
    sio_error("waitpid() failed");

  errno = old_errno;
}; /* INFO: Problem 11.8 part 1 end */

/**
 * main - Iterative Network Listener.
 * Logic: Problem 11.13. Ignores SIGPIPE to handle premature client
 * disconnections. Establishes the listen socket and enters the primary
 * accept-doit-close lifecycle.
 */
int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  signal(SIGPIPE, SIG_IGN);         /* INFO: Problem 11.13 */
  Signal(SIGCHLD, sigchld_handler); /* INFO: Problem 11.8 part 2 */

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  /* Initialize the listening socket */
  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    /* Block until a client connects */
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);  /* Process the request */
    Close(connfd); /* Close connection to prevent memory and fd leak */
  }
}

/**
 * doit - Transaction Orchestrator.
 * Logic: The central dispatcher. Coordinates RIO initialization,
 * request parsing, URI routing, and permission validation before
 * delegating to the static or dynamic subsystem.
 */
static void doit(int fd) {
  int is_static;
  struct stat sbuf;
  httpreq_t req;
  memset(&req, 0, sizeof(httpreq_t));
  rio_t rio;

  /* Bind a socket FD to the RIO state monitor */
  Rio_readinitb(&rio, fd);

  /* Parse all high level fields in the request to update RIO state */
  if (parse_request(&rio, &req) < 0)
    return;

  /* Router: Determine if request is for a file or a script */
  is_static = parse_uri(&req);
  /* OS: Check whether target exists on disk */
  if (stat(req.filename, &sbuf) < 0) {
    clienterror(fd, req.filename, 404, "Tiny couldn't find this file");
    return;
  }

  if (is_static) { /* Serve static content */
    /* Check permission for file: usr_read + regular_file */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, req.filename, 403, "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, sbuf.st_size, &req);
  } else { /* Serve dynamic content */
    /* Check permission for script: usr_exe + regular_file */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, req.filename, 403, "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, &req);
  }
}

/**
 * supportedMethod - Protocol Validator.
 * Logic: Problem 11.11. Restricts the server to idempotent (GET/HEAD)
 * and data-bearing (POST) methods, returning a boolean for the parser.
 */
static int supportedMethod(const char *method) {
  /* clang-format off */
  if (!strcasecmp(method, "GET"))     return 1;
  if (!strcasecmp(method, "HEAD"))    return 1;
  if (!strcasecmp(method, "POST"))    return 1;
  /* if (!strcasecmp(method, "PUT"))     return 1; */
  /* if (!strcasecmp(method, "PATCH"))   return 1; */
  /* if (!strcasecmp(method, "DELETE"))  return 1; */
  /* if (!strcasecmp(method, "OPTIONS")) return 1; */
  return 0;
  /* clang-format on */
}

/**
 * clienterror - HTTP Error Response Generator.
 * Logic: Constructs a valid HTTP/1.1 error header and a user-friendly
 * HTML body. Crucial for informing the client when 403, 404, or 501 occur.
 */
static void clienterror(int fd, char *cause, int errnum, char *explain) {
  char errwords[MAXLINE], body[MAXBUF], response[MAXLINE];

  switch (errnum) {
    /* clang-format off */
  case 400: strcpy(errwords, "Bad Request");     break;
  case 403: strcpy(errwords, "Forbidden");       break;
  case 404: strcpy(errwords, "Not Found");       break;
  case 501: strcpy(errwords, "Not Implemented"); break;
  default:  strcpy(errwords, "Internal error");
    /* clang-format on */
  }
  /* Build body */
  snprintf(body, MAXBUF, error_template, errnum, errwords, explain, cause);
  /* Build response head */
  snprintf(response, MAXLINE,
           "HTTP/1.1 %d %s\r\n"
           "Connection: close\r\n"
           "Content-Length: %zu\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n",
           errnum, errwords, strlen(body));

  /* Print the HTTP response headers */
  Rio_writen(fd, response, strlen(response));
  /* Print the HTTP response body */
  Rio_writen(fd, body, strlen(body));
}

/**
 * parse_request - HTTP/1.1 Header Parser.
 * Logic: Problem 11.12. Deserializes the request line and consumes
 * headers. Populates httpreq_t with Content-Length/Type and buffers
 * the POST body from the RIO stream if necessary.
 */
static int parse_request(rio_t *rp, httpreq_t *req) {
  char buf[MAXLINE] = {'\0'};
  char *start = NULL, *end = NULL;

  if (!Rio_readlineb(rp, buf, MAXLINE))
    return -1;
  printf("%s", buf);
  /* Parse request-line and validate data therein */
  if (sscanf(buf, "%s %s %s", req->method, req->uri, req->version) != 3) {
    clienterror(rp->rio_fd, buf, 400, "The request-line is incorrect");
    return -1;
  }
  /* Validate request method */
  if (!supportedMethod(req->method)) {
    clienterror(rp->rio_fd, req->method, 501,
                "Tiny does not implement this method");
    return -1;
  }
  /* Check for malicious path */
  if (strstr(req->uri, "..")) {
    clienterror(rp->rio_fd, req->uri, 404,
                "Tiny could not find the requested resource");
    return -1;
  }

  /* Check for empty-line as request head terminator */
  while (1) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    if (!strcmp(buf, "\r\n"))
      break;

    end = strpbrk(buf, "\r\n");
    *end = '\0';

    /* Parse headers and retrieve content-length */
    if (!strncasecmp(buf, "content-length:", strlen("content-length:"))) {
      start = strchr(buf, ':');
      while (!isdigit(*(++start)))
        ;
      req->content_length = strtol(start, NULL, 10);
      /* printf("\t DEBUG:\t%d\n", req->content_length); */
    } else if (!strncasecmp(buf, "content-type:", strlen("content-type:"))) {
      start = strchr(buf, ':');
      while (isspace(*(++start)))
        ;
      strncpy(req->content_type, start, MAXLINE - 1);
      /* printf("\t DEBUG:\t%s\n", req->content_type); */
    }
  }

  if (req->content_length > 0) {
    Rio_readnb(rp, req->body, req->content_length);
    /*  printf("DEBUG: ******************************\n" */
    /*          "%.*sDEBUG: ******************************\n" */
    /*          "\n", */
    /*          req->content_length, req->body); */
  }

  return 0;
}

/**
 * parse_uri - Resource Router.
 * Logic: Problem 11.7. Maps URIs to local disk paths.
 * Discriminates between static assets (public/)
 * and dynamic endpoints (cgi-bin/)
 * while extracting query parameters for GET requests.
 */
static int parse_uri(httpreq_t *req) {
  char *ptr;

  if (!strstr(req->uri, "cgi-bin")) {
    /* INFO: Problem 11.7 part 1 start */
    if (strstr(req->uri, "video/")) {
      strcpy(req->cgiargs, "");
      ptr = strrchr(req->uri, '/');
      strcpy(req->filename, video_path);
      strcat(req->filename, ptr);
      return 1;
    } /* INFO: Problem 11.7 part 1 end */
    /* Static path logic */
    strcpy(req->cgiargs, "");                  /* clear cgi */
    strcpy(req->filename, "./public");         /* begin convert1 */
    strcat(req->filename, req->uri);           /* end convert1 */
    if (req->uri[strlen(req->uri) - 1] == '/') /* slash check */
      strcat(req->filename, "index.html");     /* append default */
    return 1;
  } else {
    /* Dynamic path logic */
    ptr = strchr(req->uri, '?'); /* begin extract */
    if (ptr) {
      strcpy(req->cgiargs, ptr + 1);
      *ptr = '\0';
    } else
      strcpy(req->cgiargs, "");      /* end extract */
    strcpy(req->filename, ".");      /* begin convert2 */
    strcat(req->filename, req->uri); /* end convert2 */
    return 0;
  }
}

/**
 * serve_static - Static Asset Provider.
 * Logic: Problem 11.9 & 11.11. Maps files to memory (mmap) or reads
 * to buffer. Handles HEAD requests by skipping the body transfer
 * after the header handshake.
 */
static void serve_static(int fd, int filesize, httpreq_t *req) {
  int srcfd;
  char *srcp, filetype[4096], buf[MAXBUF];

  /* Phase 1: headers handshake */
  get_filetype(req->filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n", filesize);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
  Rio_writen(fd, buf, strlen(buf));

  if (!strcasecmp(req->method, "HEAD")) /* INFO: Problem 11.11 part 5 */
    return;
  /* Phase 2: malloc incurs double copy:
    Kernel Page Cache → User buffer → Kernel network buffer */
  srcp = Malloc(filesize); /* INFO: Problem 11.9 start */
  srcfd = Open(req->filename, O_RDONLY, 0);
  Rio_readn(srcfd, srcp, filesize);
  Close(srcfd);
  Rio_writen(fd, srcp, filesize);
  Free(srcp); /* INFO: Problem 11.9 end */
}

/**
 * get_filetype - MIME Type Resolver.
 * Logic: Problem 11.7. Maps file extensions to standard MIME strings
 * to ensure the browser's renderer handles the payload correctly.
 */
static void get_filetype(char *filename, char *filetype) {
  char *last_dot = strrchr(filename, '.');
  /* Default to binary stream if no extension is found */
  if (!last_dot) {
    strcpy(filetype, "application/octet-stream");
    return;
  }
  /* clang-format off */
  /* INFO: Problem 11.7 part 2: Video Formats */
  if      (!strcmp(last_dot, ".mp4"))  strcpy(filetype, "video/mp4");
  else if (!strcmp(last_dot, ".webm")) strcpy(filetype, "video/webm");
  else if (!strcmp(last_dot, ".ogv"))  strcpy(filetype, "video/ogg");
  else if (!strcmp(last_dot, ".mov"))  strcpy(filetype, "video/quicktime");
  /* Audio Formats (Bonus) */
  else if (!strcmp(last_dot, ".mp3"))  strcpy(filetype, "audio/mpeg");
  else if (!strcmp(last_dot, ".wav"))  strcpy(filetype, "audio/wav");
  /* Text & Code */
  else if (!strcmp(last_dot, ".html")) strcpy(filetype, "text/html");
  else if (!strcmp(last_dot, ".htm"))  strcpy(filetype, "text/html");
  else if (!strcmp(last_dot, ".css"))  strcpy(filetype, "text/css");
  else if (!strcmp(last_dot, ".js"))   strcpy(filetype, "application/javascript");
  else if (!strcmp(last_dot, ".json")) strcpy(filetype, "application/json");
  else if (!strcmp(last_dot, ".txt"))  strcpy(filetype, "text/plain");
  /* Images */
  else if (!strcmp(last_dot, ".jpg"))  strcpy(filetype, "image/jpeg");
  else if (!strcmp(last_dot, ".jpeg")) strcpy(filetype, "image/jpeg");
  else if (!strcmp(last_dot, ".png"))  strcpy(filetype, "image/png");
  else if (!strcmp(last_dot, ".gif"))  strcpy(filetype, "image/gif");
  else if (!strcmp(last_dot, ".ico"))  strcpy(filetype, "image/x-icon");
  else if (!strcmp(last_dot, ".svg"))  strcpy(filetype, "image/svg+xml");
  /* Documents */
  else if (!strcmp(last_dot, ".pdf"))  strcpy(filetype, "application/pdf");
  else if (!strcmp(last_dot, ".xml"))  strcpy(filetype, "application/xml");
  /* Fallback for unrecognized extensions */
  else                                 strcpy(filetype, "application/octet-stream");
  /* clang-format on */
}

/**
 * serve_dynamic - CGI Process Bridge.
 * Logic: Problem 11.11 & 11.12. Forks a child, sets up the CGI
 * environment (METHOD, QUERY, LENGTH), and uses a pipe to stream
 * POST bodies from parent to child stdin.
 */
static void serve_dynamic(int fd, httpreq_t *req) {
  char buf[MAXLINE], *emptylist[] = {NULL};
  int fdset[2] = {-1, -1}; /* Initialize to invalid fd. */
  int is_post = !strcmp(req->method, "POST");

  if (is_post) {
    if (pipe(fdset) < 0) {
      fprintf(stderr, "pipe() failed: %s\n", strerror(errno));
      /* TODO: Exit properly */
    }
  }

  /* Server returns only the response-line */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  /* All subsequent headers are sent by the CGI Application together with the
   * response body */

  /* Child */
  if (Fork() == 0) {
    /* Real server would set all CGI vars here */
    setenv("REQUEST_METHOD", req->method, 1); /* INFO: Problem 11.11 part */
    if (!strcmp(req->method, "GET")) {
      setenv("QUERY_STRING", req->cgiargs, 1);
    } else if (is_post) {
      snprintf(buf, MAXLINE, "%d", req->content_length);
      setenv("CONTENT_LENGTH", buf, 1);
      setenv("CONTENT_TYPE", req->content_type, 1);
    }
    /* Redirect stdout to client */
    Dup2(fd, STDOUT_FILENO);
    /* If POST request: redirect stdin to reading end of parent pipe */
    if (is_post) {
      Close(fdset[1]);
      Dup2(fdset[0], STDIN_FILENO);
      Close(fdset[0]);
    }
    /* Spawn CGI Application. */
    Execve(req->filename, emptylist, environ);
    /* Run CGI program */
    fprintf(stderr, "execve() failed\n");
    _exit(1);
  }

  /* Parent */
  if (is_post) {
    /* Send request body down the pipe to CGI Application. */
    Close(fdset[0]);
    Rio_writen(fdset[1], req->body, req->content_length);
    Close(fdset[1]);
  }
  /* Parent don't wait but reaps child in signal handler. */
}
