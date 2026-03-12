/* csapp/ch09-virtual-memory/090804-mmap/mmapcopy_v0.c */
// Created on: Mon Nov 17 17:31:52 +01 2025
// Practice problem 9.5:
// Write a C program mmapcopy.c that uses mmap to copy an arbitrary-size disk
// file to stdout. The name of the input file should be passed as a command-line
// argument.

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[argc + 1]) {
  if (argc == 1) {
    fprintf(stderr, "Usage: %s <disk_file>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  const char *filename = argv[1];

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open() failed");
    exit(EXIT_FAILURE);
  }

  off_t filelen = lseek(fd, 0, SEEK_END);
  if (filelen == -1) {
    perror("lseek() failed");
    exit(EXIT_FAILURE);
  }
  // FIX: Nor required since mmap operates from its offset argument but best
  // recommands to rewind before mmap.
  // HACK: By calling fstat() to get the file size prescind from any
  // manipulation of the file offset.

  // FIX: Using both "MAP_PRIVATE | MAP_SHARED" is illegal.
  char *ptr = mmap(NULL, filelen, PROT_READ, MAP_PRIVATE | MAP_SHARED, fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap() failed");
    exit(EXIT_FAILURE);
  }

  // FIX: Using buffer text I/O library function on arbitrary binary file is
  // calling for undefined behavior.
  printf("%.*s\n", (int)filelen, ptr);

  if (munmap(ptr, filelen) == -1) {
    perror("munmap() failed");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
