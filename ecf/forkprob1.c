/* Practice Problem 8.11 */
/* How many “hello” output lines does this program print? */

#include "../include/csapp.h"

int main() {
  int i;

  for (i = 0; i < 2; i++)
    Fork();
  printf("hello\n");
  exit(0);
}
