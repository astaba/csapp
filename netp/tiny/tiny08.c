/* =========================================================================
 * Created on: <Fri Mar 27 15:30:04 +01 2026>
 * Time-stamp: <Sun Mar 29 00:04:15 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/tiny/tiny08.c -
 *             tiny.c - A simple, iterative HTTP/1.0 Web server that uses
 *             the GET method to serve static and dynamic content.
 *
 * Problem 11.8: Modify Tiny so that it reaps CGI children inside
 * a SIGCHLD handler instead of explicitly waiting for them to terminate.
 * ========================================================================= */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *errwords,
                 char *errmsg);

static void sigchld_handler(int sig) {
  int old_errno = errno;
  pid_t pid;

  /* WARN: Don't block on wait if one child is not dead yet. */
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    Sio_puts("Handler reaped child.\n"); /* NOTE: No good on real server. */
  }
  /* WARN: Only ever checks errno IFF an error occured. */
  if ((pid < 0) && (errno != ECHILD))
    sio_error("waitpid() failed");

  errno = old_errno;
}

/**
 * main - Figure 11.29: Iterative HTTP/1.0 server loop.
 * Creates listening socket, accepts client connections,
 * logs client info, delegates each request to doit(),
 * then closes the connection.
 * @args : port (argv[1]) - The port to listen on.
 * @return: 0 on success (though this server runs infinitely).
 */
int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Register portable signal handler */
  Signal(SIGCHLD, sigchld_handler);

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
    doit(connfd); /* Process the request */
    printf("Closing connection...\n");
    Close(connfd); /* Close connection to prevent memory and fd leak */
  }
}

/**
 * doit - Figure 11.30: Handle one HTTP transaction.
 * Role: Reads request line/headers, parses URI,
 * distinguishes static vs dynamic content, checks permissions,
 * and calls serve_static() or serve_dynamic().
 * @arg fd: The connected socket file descriptor.
 * @return: void (Handles error reporting internally via clienterror).
 */
void doit(int fd) {
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* Bind a socket FD to the RIO state monitor */
  Rio_readinitb(&rio, fd);
  /* First read only the request-line: validate method, extract URI */
  if (!Rio_readlineb(&rio, buf, MAXLINE)) /* Read in request line */
    return;
  printf("%s", buf);
  /* Parse the request line into its three components */
  /* Dynamic example: "GET /cgi-bn/adder?12&18 HTTP/1.1" */
  /* Static example: "GET / HTTP/1.1" */
  sscanf(buf, "%s %s %s", method, uri, version);
  /* Validate request method */
  if (strcasecmp(method, "GET")) {
    clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
    return;
  }
  /* Flush in request headers to update RIO state */
  read_requesthdrs(&rio);

  /* Router: Determine if request is for a file or a script */
  is_static = parse_uri(uri, filename, cgiargs);
  /* OS: Check whether target exists on disk */
  if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }

  if (is_static) { /* Serve static content */
    /* Check permission for file: usr_read + regular_file */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size);
  } else { /* Serve dynamic content */
    /* Check permission for script: usr_exe + regular_file */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}

/**
 * clienterror - Figure 11.31: Send HTTP error response.
 * Role: builds headers + HTML body describing error.
 * @arg fd: Client socket FD.
 * @arg cause: The specific string/file causing the error.
 * @arg errnum: HTTP status code (e.g. "404").
 * @arg errwords: Short human-readable status.
 * @arg errmsg: Detailed error description.
 * @return: void.
 */
void clienterror(int fd, char *cause, char *errnum, char *errwords,
                 char *errmsg) {
  char buf[MAXLINE];

  /* Print the HTTP response headers */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, errwords);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n\r\n");
  Rio_writen(fd, buf, strlen(buf));

  /* Print the HTTP response body */
  sprintf(buf, "<html><title>Tiny Error</title>");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<body bgcolor="
               "ffffff"
               ">\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "%s: %s\r\n", errnum, errwords);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<p>%s: %s\r\n", errmsg, cause);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
  Rio_writen(fd, buf, strlen(buf));
}

/**
 * read_requesthdrs - Figure 11.32:
 * Consume and print HTTP request headers.
 * Reads until blank line terminator, discarding values.
 * Role: advances rio buffer past headers for doit().
 * @arg rp: Pointer to the initialized RIO buffer.
 * @return: void.
 */
void read_requesthdrs(rio_t *rp) {
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  printf("%s", buf);
  /* Check for empty-line as request head terminator */
  while (strcmp(buf, "\r\n")) {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

/**
 * parse_uri - Figure 11.33:
 * Split URI into filename and CGI args.
 * Role: tells doit() whether to serve file or run CGI.
 * @arg uri: The raw URI string from the request.
 * @arg filename: [out] Buffer to store the mapped local path.
 * @arg cgiargs: [out] Buffer to store CGI parameters.
 * @return: 1 if static content, 0 if dynamic (cgi-bin).
 */
int parse_uri(char *uri, char *filename, char *cgiargs) {
  char *ptr;

  if (!strstr(uri, "cgi-bin")) {
    /* Static path logic */
    strcpy(cgiargs, "");             /* clear cgi */
    strcpy(filename, ".");           /* begin convert1 */
    strcat(filename, uri);           /* end convert1 */
    if (uri[strlen(uri) - 1] == '/') /* slash check */
      strcat(filename, "home.html"); /* append default */
    return 1;
  } else {
    /* Dynamic path logic */
    ptr = strchr(uri, '?'); /* begin extract */
    if (ptr) {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    } else
      strcpy(cgiargs, ""); /* end extract */
    strcpy(filename, "."); /* begin convert2 */
    strcat(filename, uri); /* end convert2 */
    return 0;
  }
}

/**
 * serve_static - Figure 11.34: Send file contents to client.
 * Role: builds HTTP headers, memory-maps file, writes body.
 * @arg fd: Client socket FD.
 * @arg filename: Path to the file on disk.
 * @arg filesize: Total size in bytes (from stat).
 * @return: void.
 */
void serve_static(int fd, char *filename, int filesize) {
  int srcfd;
  char *srcp, filetype[4096], buf[MAXBUF];

  /* Phase 1: headers handshake */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n", filesize);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: %s\r\n\r\n", filetype);
  Rio_writen(fd, buf, strlen(buf));

  /* Phase 2: Zero-copy-style Body Delivery */
  srcfd = Open(filename, O_RDONLY, 0);
  /* Map file to virtual memory: srcp points to the start of the file in RAM */
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  Close(srcfd);
  Rio_writen(fd, srcp, filesize);
  Munmap(srcp, filesize);
}

/**
 * get_filetype - Infer MIME type from filename.
 * Role: used by serve_static() to set Content-Type header.
 * @arg filename: Input string.
 * @arg filetype: [out] Buffer to store the MIME string.
 * @return: void.
 */
void get_filetype(char *filename, char *filetype) {
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else
    strcpy(filetype, "text/plain");
}

/**
 * serve_dynamic - Figure 11.35:
 * Execute CGI program and stream output.
 * Role: forks child, sets QUERY_STRING, redirects stdout to client,
 * execs CGI, parent waits.
 * @arg fd: Client socket FD.
 * @arg filename: Path to the executable.
 * @arg cgiargs: Parameters passed via the URI.
 * @return: void.
 */
void serve_dynamic(int fd, char *filename, char *cgiargs) {
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0) { /* Child */
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO);
    /* Redirect stdout to client */
    Execve(filename, emptylist, environ);
    /* Run CGI program */
  }
  /* NOTE: Children are reaped through signal handler.
     Parent do not need to wait for child to exit. */
}
