/* =========================================================================
 * Created on: <Wed Mar 18 17:38:10 +00 2026>
 * Time-stamp: <Thu Mar 26 21:14:00 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/youta/ytiny.c -
 * ========================================================================= */
#include "html_error.c.inc"
#include "ycsapp.h"

static int open_listenfd(const char *port);
static void transact(int connfd);
static void clienterror(int connfd, const char *cause, int errcode,
                        const char *errmsg);
static int parse_uri(char *uri, char *filename, char *cgiargs);
static void get_filetype(const char *filename, char *filetype);
static void serve_static(int connfd, const char *filename, int filesize);
static void serve_dynamic(int connfd, const char *filename,
                          const char *cgiargs);

int main(int argc, char *argv[argc + 1]) {
  /* 1. open_listenfd --------------------------------------------- */
  char *port;
  int listenfd;
  /* 2.1 main loop: accept_conn ----------------------------------- */
  struct sockaddr_storage clientaddr;
  socklen_t clientlen;
  char host_buf[MAXBUF], port_buf[MAXBUF];
  int connfd, gni_err;
  /* -------------------------------------------------------------- */

  signal(SIGPIPE, SIG_IGN);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(EXIT_SUCCESS);
  }
  port = argv[1];

  /* (1) open_listenfd(char *port) ===========================*/
  if ((listenfd = open_listenfd(port)) < 0)
    exit(EXIT_FAILURE);
  /* End of open_listenfd. Returns listenfd ================ */

  /* (2) Server main loop (int listenfd) ==================== */
  /* For lack of concurrency capabilities the server services only on
     request per iteration. */
  while (1) {
    /* 2.1 accept_conn(int listenfd) ------------------------ */
    /* Block on accept() until incoming connection then notify address
       of accepted connection and returns another connection file
       descriptor. */
    clientlen = sizeof(clientaddr);
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    if (connfd < 0) {
      fprintf(stderr, "accept() failed: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("Accepted connection from: ");
    gni_err = getnameinfo((struct sockaddr *)&clientaddr, clientlen, host_buf,
                          sizeof(host_buf), port_buf, sizeof(port_buf), 0);
    if (gni_err) {
      fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("(%s, %s)\n", host_buf, port_buf);
    /* End of accept_conn: Returns connfd ----------------- */
    transact(connfd);
    /* Release socket descriptor to prevent memory and fd leaks. */
    Yclose(connfd);
  } /* End of main loop ==================================== */

  exit(EXIT_SUCCESS);
}

/* Open listening socket file descriptor bound to local address */
static int open_listenfd(const char *port) {
  struct addrinfo hints, *adinflist, *adinf;
  int gai_err, listenfd, opval = 1;

  printf("Configure local address...\n");
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;             /* Accept TCP connection */
  hints.ai_family = AF_INET;                   /* ... using IPv4 */
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE; /* ... on any local IP address */
  hints.ai_flags |= AI_NUMERICSERV;            /* ... using numeric port */

  gai_err = getaddrinfo(NULL, port, &hints, &adinflist);
  if (gai_err) {
    fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(errno));
    return -1;
  }
  /* Walk the list for one address that can be bound to. */
  for (adinf = adinflist; adinf; adinf = adinf->ai_next) {
    /* Create a socket descriptor */
    printf("Creating socket descriptor...\n");
    listenfd = socket(adinf->ai_family, adinf->ai_socktype, adinf->ai_protocol);
    if (listenfd < 0)
      continue; /* socket() failed: try another address */
    /* We have a valid socket fd. Modify it before bind(). */
    /* Prevent bind() from throwing: "Address already in use" in case
       need arises to re-launch the server */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&opval,
                   sizeof(opval)) < 0)
      fprintf(stderr, "setcockopt() failed: %s\n", strerror(errno));
    /* Bind socket descriptor to local address */
    printf("Binding socket descriptor to local address...\n");
    if (bind(listenfd, adinf->ai_addr, adinf->ai_addrlen) == 0)
      break; /* Success */

    /* bind() failed, close the descriptor and try next address */
    Yclose(listenfd);
  } /* end of list walk */

  /* Clean up */
  freeaddrinfo(adinflist);
  if (adinf == NULL) /* There was no valid address */
    return -1;

  /* Make fd a passive socket ready to acccept connections. */
  printf("Listening to connection...\n");
  if (listen(listenfd, LISTENQ) < 0) {
    fprintf(stderr, "listen() failed: %s\n", strerror(errno));
    Yclose(listenfd);
    return -1;
  }

  return listenfd;
}

/* Handle the HTTP transaction */
static void transact(int connfd) {
  yio_t yio;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  int is_static;
  struct stat stbuf;

  /* Init YIO state manager */
  yio_initstate(&yio, connfd);
  /* Read request-line: validate method, extract URI */
  if (yio_lreadb(&yio, buf, MAXLINE) < 0) {
    fprintf(stderr, "yio_lreadb() failed: %s\n", strerror(errno));
    return;
  }
  printf("%s", buf);

  if (sscanf(buf, "%s %s %s", method, uri, version) != 3) {
    clienterror(connfd, buf, 400, "The request-line is incorrect");
    return;
  }
  if (strcasecmp(method, "GET")) { /* Check for supported methonnd */
    clienterror(connfd, method, 501, "Youtiny does not implement this method");
    return;
  }
  if (strstr(uri, "..")) { /* Check for malicious path */
    clienterror(connfd, uri, 404,
                "Youtiny could not find the requested resource");
    return;
  }

  /* Read all remaining request head and print to stdout */
  do {
    if (yio_lreadb(&yio, buf, MAXLINE) < 0) {
      fprintf(stderr, "yio_lreadb() failed: %s\n", strerror(errno));
      return;
    }
    printf("%s", buf);
  } while (strcmp(buf, "\r\n"));

  /* Router:
   * Parse uri and return boolean whether static or dynamic path
   * As output args set filename and cgiargs. */
  is_static = parse_uri(uri, filename, cgiargs);
  /* Before routing: check existence of requested resource. */
  if (stat(filename, &stbuf) < 0) {
    clienterror(connfd, filename, 404,
                "Youtiny could not find the requested resource");
    return;
  }

  if (is_static) { /* Server Static Content */
    /* Check: file_type=regular and permission=read_user  */
    if (!(S_ISREG(stbuf.st_mode)) || !(S_IRUSR & stbuf.st_mode)) {
      clienterror(connfd, filename, 403, "Youtiny could not read the file");
      return;
    }
    serve_static(connfd, filename, stbuf.st_size);
  } else { /* Serve Dynamic Content */
           /* Check: file_type=regular and permission=exec_user */
    if (!(S_ISREG(stbuf.st_mode)) || !(S_IXUSR & stbuf.st_mode)) {
      clienterror(connfd, filename, 403,
                  "Youtiny could not run the CGI program");
      return;
    }
    serve_dynamic(connfd, filename, cgiargs);
  }
}

/* Tell routing path by parsing URI. */
static int parse_uri(char *uri, char *filename, char *cgiargs) {
  int is_static;
  char *ptr;

  /* WARN: No matter the path, make sure to properly set cwd: watch out for
   * srtcat() vs strcpy() confusions. */
  strcpy(filename, ".");

  if (strstr(uri, "cgi-bin") == NULL) {
    /* Static path */
    strcpy(cgiargs, "");
    strcat(filename, "/public");
    strcat(filename, uri);
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "index.html");
    is_static = 1;
  } else {
    /* Dynamic path */
    ptr = strchr(uri, '?');
    if (ptr) {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    } else
      strcpy(cgiargs, "");
    strcat(filename, uri);
    is_static = 0;
  }
  return is_static;
}

/* Send HTML error page to client. */
static void clienterror(int connfd, const char *cause, int errcode,
                        const char *errmsg) {
  char errwords[MAXLINE], body[MAXBUF], response[MAXLINE];

  switch (errcode) {
    /* clang-format off */
    case 400: strcpy(errwords, "Bad Request");     break;
    case 403: strcpy(errwords, "Forbidden");       break;
    case 404: strcpy(errwords, "Not Found");       break;
    case 501: strcpy(errwords, "Not Implemented"); break;
    default:  strcpy(errwords, "Internal error");
    /* clang-format on */
  }

  /* Build body */
  snprintf(body, MAXBUF, error_html_template, errcode, errwords, errmsg, cause);
  /* Build response head */
  snprintf(response, MAXLINE,
           "HTTP/1.1 %d %s\r\n"
           "Connection: close\r\n"
           "Content-Length: %zu\r\n"
           "Content-Type: text/html; charset=UTF-8\r\n"
           "\r\n",
           errcode, errwords, strlen(body));
  /* Send Error response ignoring IO count if client close connection
     prematurely. */
  if (Yio_writen(connfd, response, strlen(response)) < 0) {
    fprintf(stderr, "Yio_writen() failed on error headers.\n");
    return;
  }
  if (Yio_writen(connfd, body, strlen(body)) < 0)
    fprintf(stderr, "Yio_writen() failed on error body.\n");
}

/* Get file type */
static void get_filetype(const char *filename, char *filetype) {
  char *last_dot;
  last_dot = strrchr(filename, '.');
  if (last_dot) {
    /* clang-format off */
    if (!strcmp(last_dot, ".css"))  strcpy(filetype, "text/css");
    if (!strcmp(last_dot, ".csv"))  strcpy(filetype, "text/csv");
    if (!strcmp(last_dot, ".htm"))  strcpy(filetype, "text/html");
    if (!strcmp(last_dot, ".html")) strcpy(filetype, "text/html");
    if (!strcmp(last_dot, ".txt"))  strcpy(filetype, "text/plain");
    if (!strcmp(last_dot, ".gif"))  strcpy(filetype, "image/gif");
    if (!strcmp(last_dot, ".ico"))  strcpy(filetype, "image/x-icon");
    if (!strcmp(last_dot, ".jpeg")) strcpy(filetype, "image/jpeg");
    if (!strcmp(last_dot, ".jpg"))  strcpy(filetype, "image/jpeg");
    if (!strcmp(last_dot, ".png"))  strcpy(filetype, "image/png");
    if (!strcmp(last_dot, ".svg"))  strcpy(filetype, "image/svg+xml");
    if (!strcmp(last_dot, ".js"))   strcpy(filetype, "application/javascript");
    if (!strcmp(last_dot, ".json")) strcpy(filetype, "application/json");
    if (!strcmp(last_dot, ".pdf"))  strcpy(filetype, "application/pdf");
    if (!strcmp(last_dot, ".xml"))  strcpy(filetype, "application/xml");
    /* clang-format on */
  } else {
    strcpy(filetype, "application/octet-stream");
  }
}

/* Serve static content to client socket */
static void serve_static(int connfd, const char *filename, int filesize) {
  char filetype[4096], res_headers[MAXBUF], *scr_p;
  int scrfd;

  get_filetype(filename, filetype);
  /* Phase 1: response-headers and handshake */
  snprintf(res_headers, MAXLINE,
           "HTTP/1.1 200 OK\r\n"
           "Connection: close\r\n"
           "Content-Length: %d\r\n"
           "Content-Type: %s; charset=UTF-8\r\n"
           "\r\n",
           filesize, filetype);
  /* WARN: sizeof() is wrong. Send headers with strlen() */
  if (Yio_writen(connfd, res_headers, sizeof(res_headers)) < 0) {
    fprintf(stderr, "Yio_writen() failed on static headers.\n");
    return;
  }

  /* Phase 2: Send response body */
  if ((scrfd = (open(filename, O_RDONLY, 0))) < 0)
    perror("open() failed");
  else {
    scr_p = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, scrfd, 0);
    Yclose(scrfd);
    if (scr_p == MAP_FAILED) {
      perror("mmap() failed");
    } else {
      if (Yio_writen(connfd, scr_p, filesize) < 0) {
        fprintf(stderr, "Yio_writen() failed on static content.\n");
        return;
      }
      if (munmap(scr_p, filesize) < 0)
        perror("munmap() failed");
    }
  }
}

/* Set CGI variables and Spawn dynamic content process */
static void serve_dynamic(int connfd, const char *filename,
                          const char *cgiargs) {
  char res_headers[MAXBUF], *emptyargv[] = {NULL};
  pid_t pid;
  /* Return first part of request headers */
  snprintf(res_headers, MAXLINE,
           "HTTP/1.1 200 OK\r\n"
           "Server: Youtiny Web Server\r\n");
  /* WARN: Don't use sizeof() but strlen() to send headers. */
  if (Yio_writen(connfd, res_headers, sizeof(res_headers)) < 0) {
    fprintf(stderr, "Yio_writen() failed on dynamic headers.\n");
    return;
  }

  /* Spawn CGI Application */
  if ((pid = fork()) < 0) {
    perror("fork() failed");
  } else if (pid == 0) {
    /* Set CGI variables in the environment */
    setenv("QUERY_STRING", cgiargs, 1);
    if (dup2(connfd, STDOUT_FILENO) < 0)
      perror("dup2() failed");
    else {
      /* Replace child image and run the CGI APP */
      execve(filename, emptyargv, __environ);
      /* Unless exec() failed, this statement is never reached:
         killed child without affecting parent */
      _exit(EXIT_FAILURE);
    }
  } else {
    if (wait(NULL) < 0)
      perror("wait() failed");
    /* Parent waits for and reaps child */
  }
}
