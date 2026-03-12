/* csapp/ch09-virtual-memory/s090804-mmap/sldwindow.c */
/* Create on: Tue Nov 18 12:59:12 +01 2025 */
/* Description: Implement the sliding-windows memory-mapping techniques far more
 * elegant than read() for huge files. This pattern is the standard method for
 * reading large files via mmap when the entire file cannot fit into the
 * available virtual address space or physical memory. */

#define _GNU_SOURCE
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

static void excp_worker(int errnoflag, int error, const char *fmt, va_list ap);
static void excp_syserr(const char *fmt, ...);
static void excp_syswar(const char *fmt, ...);
static void excp_cuserr(const char *fmt, ...);
static void excp_cuswar(const char *fmt, ...);

/* ======================== Main Program Logic ======================== */

int main(int argc, char *argv[argc + 1]) {
  if (argc != 2)
    excp_cuserr("Usage: %s <huge_file>", argv[0]);
  const char *filename = argv[1];

  /* Attach file descriptor to disk file index-node */
  int fd = open(filename, O_RDONLY);
  if (fd == -1)
    excp_syserr("open() failed '%s'", filename);

  /* Get file size from inode info */
  struct stat sb;
  if (fstat(fd, &sb) == -1)
    excp_syserr("fstat() failed '%s'", filename);
  off_t file_size = sb.st_size;

  /* Get system page size and define the custom window size to slide over. */
  size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
  const size_t window = 64ULL * 1024 * 1024;
  /* Track cumulative avancement after each windom map over the huge file. */
  off_t cumul_offset = 0;

  /* Main loop: Slide the window across the file */
  while (cumul_offset < file_size) {
    /* --- 1. Calculate i_window (Actual Data Length to Read) --- */
    /* Adjust i_window for the final, potentially smaller, chunk */
    size_t i_window = window; /* Iterative window */
    if (cumul_offset + (off_t)window > file_size)
      i_window = (size_t)(file_size - cumul_offset);

    /* --- 2. Calculate Alignment and Length --- */
    /* NOTE: To boost mmap performance the fd must be aligned sys-page-wise. */
    /* TRICK:To enforce offset alignment arithmetically the logic is:
     * backaligned_offset = cumul_offset - (cumul_offset % page_size) That is:
     * rounding the cumul_offset to the nearest page_size boundary. From the
     * express assumption that page_size is a power of 2, we can take avantage
     * of the better performance of binary operations to replicate the same
     * logic as follows: */
    off_t backaligned_offset = cumul_offset & ~(page_size - 1);
    off_t delta = cumul_offset - backaligned_offset;
    size_t i_length = i_window + delta; /* Iterative length to map */

    /* --- 3. mmap the aligned region --- */
    unsigned char *addr =
        mmap(NULL, i_length, PROT_READ, MAP_PRIVATE, fd, backaligned_offset);
    if (addr == MAP_FAILED)
      excp_syserr("mmap() failed on '%s'", filename);

    /* --- 4. Process Data (Skip delta, access i_window) --- */
    /* ptr: The actual start of the user's desired data window. */
    unsigned char *ptr = addr + delta;
    if (write(STDOUT_FILENO, ptr, i_window) != (ssize_t)i_window)
      excp_syserr("write() failed");

    /* --- 5. Clean Up and Advance --- */
    /* Unmap the full aligned region. */
    if (munmap(addr, i_length) == -1)
      excp_syserr("munmap() failed on '%s'", filename);
    /* Advance offset by the actual data size read in this iteration. */
    cumul_offset += i_window;
  }

  /* Close file descriptor */
  close(fd);
  return EXIT_SUCCESS;
}

/* ======================== Error Handling Functions ======================== */

/* Common worker function for thread-safe error reporting */
void excp_worker(int errnoflag, int error, const char *fmt, va_list ap) {
  char buf[MAXLINE];
  /* Format the custom message part */
  vsnprintf(buf, MAXLINE - 1, fmt, ap);
  /* If errnoflag is set, append the system error message using strerror */
  if (errnoflag) { /* Find end of current message and append ": %s" */
    snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, ": %s",
             strerror(error));
  }
  strcat(buf, "\n");
  fflush(stdout); /* ensure output order coherence in case stderr idem stdout */
  fputs(buf, stderr);
  fflush(NULL);
}

/* System error (fatal) - reports errno and exits */
void excp_syserr(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(1, errno, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

/* System warning (non-fatal) - reports errno but continues */
void excp_syswar(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(1, errno, fmt, ap);
  va_end(ap);
}

/* Custom error (fatal) - no errno report, just custom message, then exits */
void excp_cuserr(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(0, 0, fmt, ap);
  va_end(ap);
  exit(EXIT_FAILURE);
}

/* Custom warning (non-fatal) - no errno report, just custom message */
void excp_cuswar(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  excp_worker(0, 0, fmt, ap);
  va_end(ap);
}
