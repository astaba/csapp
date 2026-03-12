/* csapp/ch09-virtual-memory/s091214/hellomap.c */
/* Created on: Mon Dec  1 17:45:23 +01 2025 */
/* Homework 9.14: Given an input file hello.txt that consists of the string
 * Hello, world!\n, write a C program that uses mmap to change the contents of
 * hello.txt to Jello, world!\n. */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#define MAX_LINE 4096

void excp_worker(int errnoflag, int error, const char *fmt, va_list ap) {
  char buf[MAX_LINE];
  vsnprintf(buf, MAX_LINE - 1, fmt, ap);
  if (errnoflag)
    snprintf(buf + strlen(buf), MAX_LINE - strlen(buf) - 1, ": %s",
             strerror(error));
  strcat(buf, "\n");
  fflush(stdout);
  fputs(buf, stderr);
  fflush(NULL);
}

void excp_syserr(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(1, errno, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

void excp_cuserr(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(0, 0, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

void excp_cusext(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(0, 0, fmt, ap);
  va_end(ap);
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[argc + 1]) {
  if (argc != 2)
    excp_cuserr("Usage: %s <filename>", argv[0]);
  const char *filename = argv[1];

  // Open file for Read&Write
  int fd = open(filename, O_RDWR);
  if (fd == -1)
    excp_syserr("open() failed on '%s'", filename);

  // Get file size
  struct stat statbuf;
  if (fstat(fd, &statbuf) == -1) {
    close(fd);
    excp_syserr("fstat() failed on '%s'", filename);
  }
  size_t filesize = (size_t)statbuf.st_size;

  // WARNING: Handle empty file to avoid mmap() error or segfault while
  // dereferencing ptr.
  if (filesize == 0)
    excp_cusext("Nothing to do: '%s' is empty.", filename);

  // Mmap the file to a shared memory with Read&Write privilege
  char *ptr = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    close(fd);
    excp_syserr("mmap() failed on '%s'", filename);
  }

  // Make changes
  if (*ptr == 'H')
    *ptr = 'J';

  // Release resources and clean up.
  if (munmap(ptr, filesize) == -1) {
    close(fd);
    excp_syserr("munmap() failed");
  }
  close(fd);

  return EXIT_SUCCESS;
}
