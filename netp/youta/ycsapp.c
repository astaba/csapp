/* =========================================================================
 * Created on: <Sat Mar 21 18:34:36 +00 2026>
 * Time-stamp: <Thu Mar 26 20:45:00 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/youta/ycsapp.c -
 * ========================================================================= */
#include "ycsapp.h"

void Yclose(int fd) {
  int placeholder = close(fd);
  if (placeholder < 0) {
    perror("close() failed");
    exit(EXIT_FAILURE);
  }
}

void Ybind(int fd, struct sockaddr *addr, socklen_t addrlen) {
  int placeholder = bind(fd, addr, addrlen);
  if (placeholder < 0) {
    perror("bind() failed");
    exit(EXIT_FAILURE);
  }
}

void Ygetaddrinfo(const char *node, const char *service,
                  const struct addrinfo *hints, struct addrinfo **res) {
  int gai_err;
  gai_err = getaddrinfo(node, service, hints, res);
  if (gai_err) {
    fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(errno));
    exit(EXIT_FAILURE);
  }
}

/* Youtrust IO library */
/* THE BARTENDER ANALOGY (Conceptual Overview).
 * To understand the YIO package, imagine a Bartender serving clients:

 * 1. The Cellar (Kernel): Where the vast supply of data lives.  Trips
 * here are expensive (system calls).
 *
 * 2. The Keg (Internal Buffer): The Bartender keeps a private 8KB keg
 * (rio_t.rio_buf). He fills it once from the cellar and pours many
 * small drinks from it quickly.
 *
 * 3. The Mug (User Buffer): The destination provided by the client
 * (you).
 *
 * 4. The Pour (n/maxlen): The physical capacity of the client's
 * mug. The Bartender will never pour more than the mug can hold, even
 * if the keg has more.
 *
 * Logic Flow:
 * - Unbuffered functions (readn/writen) bypass the Bartender and go
     straight to the cellar.
 * - Buffered functions (readnb/readlineb) ask the Bartender to pour
     from the keg. If the keg is dry, the Bartender goes to the cellar
     to refill it before continuing the pour.
  ================================================================= */

/**
  yio_nread - Low-level loop around read() syscall.
  Read from source fd to usrbuf until all incoming data are received,
  by handly eventual interrupt signals.
  Ensures usrbuf capacity does not exceed its limit, preventing
  oveflow.
  @param fd: The source file descriptor to read data from.
  @param usrbuf: Pointer to user buffer to read data to.
  @param usrbufcap: User buffer size e.i., max data to read.
  */
ssize_t yio_nread(int fd, void *usrbuf, size_t usrbufcap) {
  size_t nleft = usrbufcap; /* The free space in usrbuf. */
  ssize_t nread;            /* The bytes read per syscall iteration. */
  char *bufp = usrbuf;      /* Tracks the advance of usrbuf population. */

  while (nleft > 0) {
    if ((nread = read(fd, bufp, nleft)) < 0) {
      if (errno == EINTR) /* Interrupted by sig handler. */
        nread = 0;        /* Reset iteration nread and try again. */
      else
        return -1;         /* read() set errno. */
    } else if (nread == 0) /* EOF */
      break;
    nleft -= nread;
    bufp += nread;
  }
  return (usrbufcap - nleft);
}

/**
  yio_written - Low-level loop around write() syscall.
  Repeat syscal until n data bytes, possibly less than usrbuf size,
  are deliverd to destination fd, while handling eventual interrupts.
  @param fd: Destination file descriptor to write data to.
  @param usrbuf: Pointer to user buffer to wrote data from.
  @param n: Number of bytes to write to fd,
  possibly less than or equal to usrbuf size.
  */
ssize_t yio_writen(int fd, void *usrbuf, size_t n) {
  size_t nleft = n;
  ssize_t nwritten;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nwritten = write(fd, bufp, nleft)) <= 0) {
      if (errno == EINTR) /* Interrupted by sig handler. */
        nwritten = 0;     /* Reset iteration nwritten and try again */
      else
        return -1; /* write() set errno */
    }
    nleft -= nwritten;
    bufp += nwritten;
  }
  return n;
}

void yio_initstate(yio_t *yp, int fd) {
  yp->yio_srcfd = fd;
  yp->yio_cnt = 0;
  yp->yio_tracker = yp->yio_buf;
}

static ssize_t yio_read(yio_t *yp, char *usrbuf, size_t usrbufcap) {
  ssize_t cnt;
  /* 1. While internal buffer empty read syscall to fill it. */
  while (yp->yio_cnt <= 0) {
    yp->yio_cnt = read(yp->yio_srcfd, yp->yio_buf, sizeof(yp->yio_buf));
    if (yp->yio_cnt < 0) {
      if (errno != EINTR) /* Report error condition. */
        return -1;
    } else if (yp->yio_cnt == 0) /* EOF */
      return 0;
    else /* Internal buffer populated: reset starting point */
      yp->yio_tracker = yp->yio_buf;
  }
  /* 2. If internal buffer already populated or once populated: Refill
        user buffer from internal buffer without indulging in syscall
        overhead. */
  cnt = usrbufcap;
  if (yp->yio_cnt < usrbufcap)
    cnt = yp->yio_cnt;
  /* Deliver data from internal buffer to user buffer. */
  memcpy(usrbuf, yp->yio_tracker, cnt);
  yp->yio_cnt -= cnt;     /* Keep tally of read bytes */
  yp->yio_tracker += cnt; /* Update next delivery starting point. */

  return cnt;
}

ssize_t yio_nreadb(yio_t *yp, void *usrbuf, size_t usrbufcap) {
  size_t nleft = usrbufcap;
  ssize_t nread;
  char *bufp = usrbuf;

  while (nleft > 0) {
    if ((nread = yio_read(yp, bufp, nleft)) < 0)
      return -1; /* read() set errno. */
    else if (nread == 0)
      break; /* EOF */
    nleft -= nread;
    bufp += nread;
  }

  return (usrbufcap - nleft);
}

ssize_t yio_lreadb(yio_t *yp, void *usrbuf, size_t len) {
  int n; /* Iteration count above null-terminator count. */
  int rc;
  char c;              /* Stack slot to read buffered char. */
  char *bufp = usrbuf; /* Tracks usrbuf knitting one-char/iteration. */

  for (n = 1; n < len; n++) {
    rc = yio_read(yp, &c, 1);
    if (rc == 1) {
      *bufp++ = c;
      if (c == '\n') {
        n++;
        break;
      }
    } else if (rc == 0) {
      if (n == 1)
        return 0; /* EOF, no data read. */
      else
        break; /* Some data was read. */
    } else
      return -1; /* read() set errno. */
  }

  *bufp = '\0'; /* Null-terminate string in usrbuf. */
  return n - 1; /* Return character read minus null-terminator. */
}

ssize_t Yio_writen(int fd, void *usrbuf, size_t n) {
  ssize_t rc;
  if ((rc = yio_writen(fd, usrbuf, n)) < 0) {
    if (errno == EPIPE) {
      fprintf(stderr, "Client closed connection prematurely: %s\n",
              strerror(errno));
    } else
      perror("yio_writen() failed");
    /* Yclose(fd); */
  }
  return rc;
}
