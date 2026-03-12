/* csapp/ch07-linking/s070601-duplicate-symbols/foo4.c */
// Created on: Wed Nov 19 13:06:16 +01 2025

#include <stdio.h>
void f(void);

int x;

int main() {
  x = 15213;
  f();
  printf("x = %d\n", x);
  return 0;
}
