/* =========================================================================
 * Created on: <Fri Mar 27 21:55:23 +01 2026> 
 * Time-stamp: <Fri Mar 27 22:12:41 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/ysystem.c -
 * Problem 8.22
 * Write your own version of the Unix system() function.
 * More details: [[file:README.org::#problem-8-22]]
 * ========================================================================= */
#include "../include/csapp.h" /* IWYU pragma: keep */

static int mysystem(char *command);

int main() {
  int status = mysystem("ls -l /tmp");
  if (status == -1) {
    fprintf(stderr, "ERROR: mysystem(): %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("\nCommand exit status: %d\n", status);
  exit(EXIT_SUCCESS);
}

static int mysystem(char *command) {
  pid_t pid;
  int status;
  const char *shell_path;

  shell_path = getenv("SHELL");
  if (shell_path == NULL)
    shell_path = "/bin/sh";

  if (command == NULL)
    return (access(shell_path, X_OK) == 0) ? 1 : 0;

  if ((pid = fork()) < 0)
    return -1; /* errno set by fork */

  if (pid == 0) {
    execl(shell_path, shell_path, "-c", command, (char *)NULL);
    _exit(127); /* exec failed */
  }

  if (waitpid(pid, &status, 0) < 0)
    return -1; /* errno set by waitpid */

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  } else {
    return status; /* abnormal termination */
  }
}
