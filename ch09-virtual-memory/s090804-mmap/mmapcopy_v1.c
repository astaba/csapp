/* csapp/ch09-virtual-memory/090804-mmap/mmapcopy_v1.c */
// Created on: Mon Nov 17 18:26:27 +01 2025
// Practice problem 9.5:
// Write a C program mmapcopy.c that uses mmap to copy an arbitrary-size disk
// file to stdout. The name of the input file should be passed as a command-line
// argument.

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

/**
 * mmapcopy - copy bytes from a file descriptor using memory mapping
 *
 * Maps up to 'len' bytes from the file (referenced by the open file descriptor
 * 'fd') into memory and copies that data to the caller's chosen output (the
 * implementation uses POSIX I/O such as write()). This function is intended to
 * perform a read/copy using mmap(2) for efficient, page-aligned access rather
 * than repeated read(2) calls.
 */
void mmapcopy(int fd, off_t len) {
  // Since we only intend to read the MAP_PRIVATE is enough
  char *ptr = mmap(NULL, (size_t)len, PROT_READ, MAP_PRIVATE, fd, 0);
  if (ptr == MAP_FAILED)
    excp_syserr("mmap() failed");

  if (write(STDOUT_FILENO, ptr, len) != len)
    excp_syserr("write() failed");

  if (munmap(ptr, len) == -1)
    excp_syserr("munmap() failed");
}

int main(int argc, char *argv[argc + 1]) {
  if (argc == 1)
    excp_cuserr("Usage: %s <disk_file>\n", argv[0]);

  const char *filename = argv[1];

  int fd = open(filename, O_RDONLY);
  if (fd == -1)
    excp_syserr("open() failed");

  struct stat sb;
  if (fstat(fd, &sb) == -1)
    excp_syserr("fstat() failed");

  mmapcopy(fd, sb.st_size);

  close(fd);

  return EXIT_SUCCESS;
}
