/* =========================================================================
 * Created on: <Fri Mar 13 14:43:33 +00 2026>
 * Time-stamp: <Mon Mar 16 12:26:42 +00 2026 by owner>
 * Author    : Astar Bahouidi
 * Desc      : Practice sandbox for CS:APP custom library.
 * ========================================================================= */
#include "../include/csapp.h"

/* ****************************************************************** */
/* Youtrust IO library */
/* THE BARTENDER ANALOGY (Conceptual Overview):

To understand the YIO package, imagine a Bartender serving clients:
   1. The Cellar (Kernel): Where the vast supply of data lives. Trips
  here are expensive (system calls).
   2. The Keg (Internal Buffer): The Bartender keeps a private 8KB keg
  (rio_t.rio_buf). He fills it once from the cellar and pours many
  small drinks from it quickly.
   3. The Mug (User Buffer): The destination provided by the client
   (you).
   4. The Pour (n/maxlen): The physical capacity of the client's
  mug. The Bartender will never pour more than the mug can hold, even
  if the keg has more.

 Logic Flow:
   - Unbuffered functions (readn/writen) bypass the Bartender and go
  straight to the cellar.
   - Buffered functions (readnb/readlineb) ask the Bartender to pour
  from the keg. If the keg is dry, the Bartender goes to the cellar to
  refill it before continuing the pour.
  ================================================================= */

/**
 * yio_readn - Youtrustly unbuffered-read n bytes into user buffer.
 *   @fd: file descriptor source.
 *   @usrbuf: user buffer (the "mug to fill").
 *   @n: capacity of user buffer.
 *   Returns: Number of bytes read (n if successful), 0 on EOF, -1 on error.
 *   Role: Low-level loop around read(), ensures full transfer unless EOF/error.
 */
ssize_t yio_readn(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nread = read(fd, bufp, nleft)) < 0) {
      if (errno == EINTR) /* Interrupted by sig handler */
        nread = 0;        /* Reset iteration nread and try again */
      else
        return -1; /* errno set by read() */
    } else if (nread == 0)
      break;
    nleft -= nread;
    bufp += nread;
  }
  return (n - nleft);
}

/**
 * yio_writen - Youtrustly unbuffered-write n bytes from user buffer.
 *   @fd: outgoing file descriptor distination.
 *   @usrbuf: user buffer (the "mug to pour out").
 *   @n: size of user buffer (how many bytes to send out).
 *   Returns: n on success, -1 on error.
 *   Role: Low-level loop around write(), ensures full transfer unless error.
 */
ssize_t yio_writen(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nwritten = write(fd, bufp, nleft)) <= 0) {
      if (errno == EINTR) /* Interrupted by sig handler */
        nwritten = 0;     /* Reset iteration nwritten and try again */
      else
        return -1; /* errno set by write() */
    }
    nleft -= nwritten;
    bufp += nwritten;
  }
  return n;
}

/**
 * yio_read - Internal buffered reader.
 *   @rp: YIO buffer state (the "keg").
 *   @usrbuf: user buffer (the mug to fill).
 *   @n: capacity of usrbuf (max bytes to pour this round).
 *   Return: min(n, rp->rio_cnt) on success, 0 on EOF, -1 on error.
 *   Role: refills keg from fd when empty, then pours up to n bytes
 *   into usrbuf. As long as it is called with the same rp, it
 *   provides buffered IO shielding the caller from expensive
 *   syscalls.  Buffering engine for higher-level routines.
 */
static ssize_t yio_read(rio_t *rp, char *usrbuf, size_t n) {
  int cnt;
  while (rp->rio_cnt <= 0) {
    rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
    if (rp->rio_cnt < 0) {
      if (errno != EINTR) /* report error condition */
        return -1;
    } else if (rp->rio_cnt == 0) /* EOF */
      return 0;
    else /* Keg is full, reset bufptr for next one-time syscall */
      rp->rio_bufptr = rp->rio_buf;
  }
  /* Refill user buffer in min(n, rp->rio_cnt) limits */
  cnt = n;
  if (rp->rio_cnt < n)
    cnt = rp->rio_cnt;
  /* Deliver buffered data to user in the limit of provided capacity */
  memcpy(usrbuf, rp->rio_bufptr, cnt);
  rp->rio_bufptr += cnt; /* Reset next delivery starting point */
  rp->rio_cnt -= cnt;    /* Update Keg state accordingly */
  return cnt;
}

/**
 * yio_readinitb - Initialize YIO buffer state manager.
 *   @rp: YIO struct to set up.
 *   @fd: file descriptor to associate buffer (the ’Keg’) with.
 *   Role: bind descriptor to a backdrop buffer for syscall IO, resets
 *   counters/pointers
 */
void yio_readinitb(rio_t *rp, int fd) {
  rp->rio_fd = fd;
  rp->rio_cnt = 0;
  rp->rio_bufptr = rp->rio_buf;
}

/**
 * yio_readnb - Youtrustly read exactly n bytes into usrbuf (buffered).
 *   @rp: YIO buffer state (keg).
 *   @usrbuf: user buffer (mug).
 *   @n: capacity of usrbuf (how many bytes to fill).
 *   Role: buffered counterpart to rio_readn, uses yio_read internally.
 */
ssize_t yio_readnb(rio_t *rp, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nread = yio_read(rp, bufp, nleft)) < 0)
      return -1; /* errno set by read() */
    else if (nread == 0)
      break; /* EOF */
    nleft -= nread;
    bufp += nread;
  }
  return (n - nleft); /* Return >= 0 */
}

/**
 * yio_readlb - Youtrustly read one text line into usrbuf (buffered).
 *   @rp: YIO buffer state (keg).
 *   @usrbuf: user buffer (mug).
 *   @len: capacity of usrbuf (maximum line length to fit).
 *   Role: line-oriented buffered reader, builds on rio_read to deliver
 *         exactly one line (ending with '\n') or EOF.
 */
ssize_t yio_readlb(rio_t *rp, void *usrbuf, size_t len) {
  int n, rc;
  char c;              /* Stack slot to read buffered char into */
  char *bufp = usrbuf; /* Tracker knit usrbuf one-char/iteration */

  for (n = 1; n < len; n++) {
    if ((rc = yio_read(rp, &c, 1)) == 1) {
      *bufp++ = c;
      if (c == '\n') {
        n++;
        break;
      }
    } else if (rc == 0) {
      if (n == 1)
        return 0; /* EOF, no data read */
      else
        break; /* Some data was read */
    } else
      return -1; /* erro set by read() */
  }

  *bufp = '\0'; /* Null-terminate string in usrbuf */
  return n - 1; /* Return characters read minus null-terminator */
}

/**********************************
 * Wrappers for robust I/O routines
 **********************************/
ssize_t Yio_readn(int fd, void *ptr, size_t nbytes) {
  ssize_t n;

  if ((n = yio_readn(fd, ptr, nbytes)) < 0)
    unix_error("Yio_readn error");
  return n;
}

void Yio_writen(int fd, void *usrbuf, size_t n) {
  if (yio_writen(fd, usrbuf, n) != n)
    unix_error("Yio_writen error");
}

void Yio_readinitb(rio_t *rp, int fd) { yio_readinitb(rp, fd); }

ssize_t Yio_readnb(rio_t *rp, void *usrbuf, size_t n) {
  ssize_t rc;

  if ((rc = yio_readnb(rp, usrbuf, n)) < 0)
    unix_error("Yio_readnb error");
  return rc;
}

ssize_t Yio_readlb(rio_t *rp, void *usrbuf, size_t len) {
  ssize_t rc;

  if ((rc = yio_readlb(rp, usrbuf, len)) < 0)
    unix_error("Yio_readlineb error");
  return rc;
}

/* ****************************************************************** */
/* Client-server helper function */

/**
  yopen_clientfd - Establishes a connection with a server. This
  function is protocol-independent (IPv4 vs IPv6). It uses getaddrinfo
  to resolve a hostname/port, then iterates through the resulting list
  of addresses until it successfully creates a socket and connects to
  a server.

  @hostname:   Hostname or IP address string.
  @port:   Service name or numeric port string.

  Returns: A valid socket descriptor (clientfd) on success.
  -1 on system error (socket/connect/close failures).
  -2 on name resolution error (getaddrinfo/getnameinfo failures).
 */
int yopen_clientfd(char *hostname, char *port) {
  int clientfd, gai_err, gni_err;
  struct addrinfo hints, *adinf, *adinflist;
  char host_buf[MAXBUF], serv_buf[MAXBUF];

  printf("onfiguring remote address...\n");
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM; /* Open TCP connection */
  hints.ai_flags = AI_NUMERICSERV; /* using a numeric port */
  hints.ai_flags |= AI_ADDRCONFIG; /* Recommended for TCP */
  /* Get a list of potential server addresses */
  gai_err = getaddrinfo(hostname, port, &hints, &adinflist);
  if (gai_err != 0) {
    fprintf(stderr, "getadrinfo() failed: %s\n", gai_strerror(gai_err));
    return -2;
  }

  printf("Opening client file descriptor...\n");
  /* Walk the list for one the can successfully connect to */
  for (adinf = adinflist; adinf; adinf = adinf->ai_next) {
    /* Create a socket descriptor */
    clientfd = socket(adinf->ai_family, adinf->ai_socktype, adinf->ai_protocol);
    if (clientfd < 0) /* socket() failed */
      continue;       /* Try another addrinfo */
    /* Connect to server */
    if (connect(clientfd, adinf->ai_addr, adinf->ai_addrlen) == 0) {
      /* Get server socket numeric host:port useful for debugging */
      printf("Client connected to remote address:\n");
      gni_err = getnameinfo(adinf->ai_addr, adinf->ai_addrlen, host_buf, MAXBUF,
                            serv_buf, MAXBUF, 0);
      if (gni_err != 0) {
        fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(gni_err));
        return -2;
      }
      printf("\t%s:%s\n", host_buf, serv_buf);
      break;                          /* Success */
    } else if (close(clientfd) < 0) { /* Connect failed, try another */
      fprintf(stderr, "close() failed: %s\n", strerror(errno));
      return -1;
    }
  }
  /* Clean up */
  freeaddrinfo(adinflist);
  if (adinf == NULL) /* All connection failed */
    return -1;
  else /* The last connect() succeeded */
    return clientfd;
}

/**
 * yopen_listenfd - Opens and returns a listening socket for a server.
 *
 * This function creates a "passive" socket ready to accept incoming
 * connection requests on all available network interfaces (IPv4/IPv6).
 * It uses the AI_PASSIVE flag to automate wildcard address binding and
 * applies SO_REUSEADDR to permit immediate restarts after a crash or exit.
 *
 * @port:    Numeric port string (e.g., "8080").
 *
 * Returns:  A valid listening socket descriptor on success.
 * -1 on system error (socket/bind/listen failures).
 * -2 on name resolution error (getaddrinfo failure).
 */
int yopen_listenfd(char *port) {
  struct addrinfo hints, *adinf, *adinflist;
  int listenfd = -1, gai_err, optval = 1;

  printf("Configuring local address...\n");
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;             /* Open TCP connection */
  hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE; /* on any IP Address */
  hints.ai_flags |= AI_NUMERICSERV;            /* using a port number */
  /* Get a list of potential server address */
  gai_err = getaddrinfo(NULL, port, &hints, &adinflist);
  if (gai_err != 0) {
    fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(gai_err));
    return -2;
  }

  printf("Opening listening file descriptor...\n");
  /* Walk the list for one the successfulle connect to */
  for (adinf = adinflist; adinf; adinf = adinf->ai_next) {
    /* Create socket descriptor */
    listenfd = socket(adinf->ai_family, adinf->ai_socktype, adinf->ai_protocol);
    if (listenfd < 0) /* socket() failed */
      continue;       /* Try another addrinfo */

    /* Prevent bind() from throwing "Address already in use" error */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optval,
                   sizeof(optval)) < 0)
      fprintf(stderr, "setsockopt() failed: %s\n", strerror(errno));

    if (bind(listenfd, adinf->ai_addr, adinf->ai_addrlen) == 0)
      break;                   /* Success */
    if (close(listenfd) < 0) { /* Bind failed, try the next */
      fprintf(stderr, "close() failed: %s\n", strerror(errno));
      return -1;
    }
  }

  /* Clean up */
  freeaddrinfo(adinflist);
  if (adinf == NULL) /* No address available */
    return -1;

  /* Make fd a passive socket ready to accept connections */
  if (listen(listenfd, LISTENQ) < 0) {
    fprintf(stderr, "listen() failed: %s\n", strerror(errno));
    close(listenfd);
    return -1;
  }
  return listenfd;
}

/****************************************************
 * Wrappers for reentrant protocol-independent helpers
 ****************************************************/
int Yopen_clientfd(char *hostname, char *port) {
  int rc;

  if ((rc = yopen_clientfd(hostname, port)) < 0)
    unix_error("Yopen_clientfd error");
  return rc;
}

int Yopen_listenfd(char *port) {
  int rc;

  if ((rc = yopen_listenfd(port)) < 0)
    unix_error("Yopen_listenfd error");
  return rc;
}
