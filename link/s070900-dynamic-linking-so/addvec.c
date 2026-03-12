/* csapp/ch07-linking/s070900-dynamic-linking-so/addvec.c */
// Created on: Sat Nov 22 22:32:44 +01 2025
// Figure 7.6.(a)
// Description: Operate on same dimension array. Sum up same-index elements from
// arrays x and y into the same-index elemet at array z.

int addcnt = 0;

void addvec(int *x, int *y, int *z, int n) {
  int i;

  addcnt++;

  for (i = 0; i < n; i++)
    z[i] = x[i] + y[i];
}
