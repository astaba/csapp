/* csapp/ch07-linking/s070900-dynamic-linking-so/multvec.c */
// Created on: Sat Nov 22 22:32:44 +01 2025
// Figure 7.6.(b)
// Description: Operate on same dimension array.  Save product of multiplying
// same-index elements from arrays x and y into the same-index elemet at array
// z.

int multcnt = 0;

void multvec(int *x, int *y, int *z, int n) {
  int i;

  multcnt++;

  for (i = 0; i < n; i++)
    z[i] = x[i] * y[i];
}
