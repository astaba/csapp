/* csapp/ch07-linking/s070601-duplicate-symbols/foo3.c */
// Created on: Wed Nov 19 12:56:39 +01 2025

#include <stdio.h>
void f(void);

int x = 15213;

int main() {
  f();
  printf("x = %d\n", x);
  return 0;
}
