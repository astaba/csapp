/* csapp/ch07-linking/s070602-static-libraries/main2.c */
// Created on: Wed Nov 19 14:26:02 +01 2025
// Figure 7.7

#include "vector.h"
#include <stdio.h>

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2];

int main() {
  addvec(x, y, z, 2);
  printf("z = [%d %d]\n", z[0], z[1]);
  return 0;
}
