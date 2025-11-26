/* csapp/ch07-linking/s070100-compiler-driver/sum.c */
// Created on: Tue Nov 18 18:15:34 +01 2025

int sum(int *a, int n) {
  int i, s = 0;

  for (i = 0; i < n; i++)
    s += a[i];

  return s;
}
