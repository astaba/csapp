/* Problem 8.20 */
#include "../include/csapp.h" // IWYU pragma: keep

int main(int argc, char *argv[], char *envp[]) {
  char *ls_path = "/bin/ls";

  if (execve(ls_path, argv, envp) == -1) {
    perror("execve() failed");
    exit(EXIT_FAILURE);
  }
}
