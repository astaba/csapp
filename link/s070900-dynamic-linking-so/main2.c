/* csapp/ch07-linking/s070900-dynamic-linking-so/main2.c */
// Created on: Sat Nov 22 22:32:44 +01 2025
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
