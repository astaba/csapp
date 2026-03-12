/* csapp/ch07-linking/s070601-duplicate-symbols/foo5.c */
// Created on: Wed Nov 19 13:10:07 +01 2025

#include <stdio.h>
void f(void);

int y = 15212;
int x = 15213;

int main() {
  f();
  printf("x = 0x%x y = 0x%x \n", x, y);
  return 0;
}
