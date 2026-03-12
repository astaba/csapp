/* csapp/ch09-virtual-memory/090804-mmap/mmapcopy_v1.c */
/* Created on: Mon Nov 17 18:26:27 +01 2025 */

/* Practice problem 9.5: Write a C program mmapcopy.c that uses mmap to copy an
 * arbitrary-size disk file to stdout. The name of the input file should be
 * passed as a command-line argument.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAXLINE 4096

static void excp_worker(int errnoflag, int error, char *fmt, va_list ap);
static void excp_syserr(char *fmt, ...);
static void excp_cuserr(char *fmt, ...);

/**
 * mmapcopy - copy bytes from a file descriptor using memory mapping
 *
 * Maps up to 'len' bytes from the file (referenced by the open file descriptor
 * 'fd') into memory and copies that data to the caller's chosen output (the
 * implementation uses POSIX I/O such as write()). This function is intended to
 * perform a read/copy using mmap(2) for efficient, page-aligned access rather
 * than repeated read(2) calls.
 */
static int mmapcopy(int fd, off_t length);

int main(int argc, char *argv[argc + 1]) {
  if (argc == 1)
    excp_cuserr("Usage: %s <disk_file>", argv[0]);

  const char *filename = argv[1];

  int fd = open(filename, O_RDONLY);
  if (fd == -1)
    excp_syserr("open() failed on '%s'", filename);
  printf("hello")
  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1)
    excp_syserr("fstat() failed on '%s'", filename);

  if (mmapcopy(fd, statbuf.st_size) == -1)
    return EXIT_FAILURE;

  close(fd);

  return EXIT_SUCCESS;
}

int mmapcopy(int fd, off_t len) {
  char *ptr = mmap(NULL, (size_t)len, PROT_READ, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap() failed");
    return -1;
  }

  if (write(STDOUT_FILENO, ptr, len) != len) {
    perror("write() failed");
    return -1;
  }

  if (munmap(ptr, len) == -1) {
    perror("munmap() failed");
    return -1;
  }
  return 0;
}

void excp_worker(int errnoflag, int error, char *fmt, va_list ap) {
  char buf[MAXLINE];
  vsnprintf(buf, MAXLINE - 1, fmt, ap);
  if (errnoflag)
    snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s",
             strerror(error));
  strcat(buf, "\n");
  fflush(stdout);
  fputs(buf, stderr);
  fflush(NULL);
}

void excp_syserr(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(1, errno, fmt, ap);
  exit(EXIT_FAILURE);
}

void excp_cuserr(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(0, 0, fmt, ap);
  exit(EXIT_FAILURE);
}
