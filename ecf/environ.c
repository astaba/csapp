/* Created on: Thu Feb 12 14:19:42 +01 2026 */
/*
  PROCESS INITIALIZATION AND ENVIRONMENT BRIDGE
  ---------------------------------------------
  1. KERNEL SETUP:
  When execve() is called, the Kernel sets up the initial process
  stack.  It pushes the environment strings and pointers onto the
  very top of the user stack before the process starts execution.

  2. LIBC ENTRY (_start):
  The execution does not start at main(), but at _start (provided by crt1.o).
  The _start routine extracts the addresses of the environment variables
  directly from the stack locations established by the Kernel.

  3. GLOBAL 'environ' (.data):
  The C runtime library (libc) defines the global variable 'environ'.
  The _start routine (or the internal __libc_start_main) initializes
  this .data segment pointer to point to the environment array on the stack.
  Crucially: 'environ' is set FROM the kernel's stack setup; main()
  receives its arguments later from this same chain.

  4. MAIN ARGUMENTS:
  Finally, main() is called with the stack-based pointers:
  [argc] -> [argv pointers] -> [NULL] -> [envp pointers] -> [NULL]

  This program prints both to show they point to the same stack-allocated
  environment block.
 */

#include <stdio.h>

// This lives in the .data segment of libc, initialized by the crt (C Runtime)
extern int **environ;

int main(int argc, char *argv[argc + 1], char *envp[]) {
  // These addresses will typically be identical or extremely close,
  // as they both point into the environment block on the initial stack.
  printf("environ (global .data): '%p'\n", environ);
  printf("envp    (stack param):  '%p'\n", envp);

  return 0;
}
