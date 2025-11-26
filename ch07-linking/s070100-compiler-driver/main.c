/* csapp/ch07-linking/s070100-compiler-driver/main.c */
// Created on: Tue Nov 18 18:12:41 +01 2025

int sum(int *a, int n);

int array[2] = {36, 15};

int main() {

  int val = sum(array, 2);

  return val;
}
