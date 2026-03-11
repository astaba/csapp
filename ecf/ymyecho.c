/* Created on: Thu Feb 12 15:13:47 +01 2026 */
/* Practice Problem 8.6 (solution page 833): Write a program called
myecho that prints its command-line arguments and environment
variables. This program shows the difference between the golbal
'environ' form the .data segment and the 'envp' as an argument of the
'main()' function. */

#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(int argc, char *argv[], char *envp[]) {
  size_t i;
  printf("--- INITIAL STATE ---\n");
  printf("envp    (stack): %p\n", (void *)envp);
  printf("environ (.data): %p\n", (void *)environ);

  /* 1. Check if they point to the same array initially */
  if (envp == environ) {
    printf("Result: 'envp' and 'environ' point to the same stack address.\n");
  }
  /* 2. force the environment to grow significantly. This will exceed the
   * original stack allocation for the env array. */
  printf("\n--- MODIFYING ENVIRONMENT ---\n");
  for (i = 0; i < 100; i++) {
    char name[10], value[10];
    sprintf(name, "NAME_%03d", (int)i);
    sprintf(value, "VALUE_%03d", (int)i);
    setenv(name, value, 1);
  }

  printf("Original envp (stack): %p\n", (void *)envp);
  printf("New environ   (.data): %p\n", (void *)environ);

  if (envp != environ) {
    printf("\nResult: 'environ' has migrated to the HEAP!\n");
    printf("The original 'envp' argument is now STALE/OUTDATED.\n");
  }

  return EXIT_SUCCESS;
}
