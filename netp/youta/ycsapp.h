/* =========================================================================
 * Created on: <Sat Mar 21 21:50:00 +00 2026>
 * Time-stamp: <Thu Mar 26 20:44:45 +01 2026 by owner>
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/netp/youta/ycsapp.h -
 * ========================================================================= */
#ifndef YCSAPP_H
#define YCSAPP_H

#include <asm-generic/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define LISTENQ 1024
#define MAXBUF 8192
#define YIO_BUFSIZE 0x2000
#define MAXLINE 0x2000

void Yclose(int fd);
void Ygetaddrinfo(const char *node, const char *service,
                  const struct addrinfo *hints, struct addrinfo **res);

/**
  Youtrust IO struct to hold and monitor the state of an internal buffer
  iteratively used to relieve user application from repetitive and
  and performance-costly syscalls every YIO_BUFSIZE bytes.
 */
typedef struct {
  int yio_srcfd;     /* Source file descriptor for this internal buffer. */
  int yio_cnt;       /* Populated size in internal bffer. */
  char *yio_tracker; /* Keep track of unpopulated starting point. */
  char yio_buf[YIO_BUFSIZE]; /* Internal buffer. */
} yio_t;

/* YIO Library */
ssize_t yio_nread(int fd, void *usrbuf, size_t usrbufcap);
ssize_t yio_writen(int fd, void *usrbuf, size_t n);
void yio_initstate(yio_t *yp, int fd);
ssize_t yio_nreadb(yio_t *yp, void *usrbuf, size_t usrbufcap);
ssize_t yio_lreadb(yio_t *yp, void *usrbuf, size_t len);

ssize_t Yio_writen(int fd, void *usrbuf, size_t n);

#endif	/* YSCAPP_H */
