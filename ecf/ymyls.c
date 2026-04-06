/* =========================================================================
 * Created on: <Fri Mar 27 21:44:40 +01 2026> 
 * Time-stamp: <Fri Mar 27 21:47:51 +01 2026 by owner> 
 * Author    : owner
 * Desc      : ~/coding/c_prog/csapp/ecf/ymyls.c -
 * Problem 8.20
 * Use execve to write a program called =myls= whose behavior is
 * identical to the =/bin/ls= program.
 * More details: [[file:README.org::#problem-8-20]]
 * ========================================================================= */
#include "../include/csapp.h" // IWYU pragma: keep

int main(int argc, char *argv[], char *envp[]) {
  char *ls_path = "/bin/ls";

  if (execve(ls_path, argv, envp) == -1) {
    perror("execve() failed");
    exit(EXIT_FAILURE);
  }
}
