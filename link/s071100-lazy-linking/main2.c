/* csapp/ch07-linking/s071100-lazy-linking/main2.c */
// Created on: Sat Nov 22 23:59:19 +01 2025
// Figure 7.17
// Before running this program make sure to build the dll.so:
// $ make dynamlib/libvector.so

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

int x[2] = {1, 2};
int y[2] = {3, 4};
int z[2];

int main() {
  void *handle;
  void (*fctn_ptr)(int *, int *, int *, int);

  // Dynamically load the shared library containing addvec()
  handle = dlopen("./dynamlib/libvector.so", RTLD_LAZY | RTLD_GLOBAL);
  if (handle == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  // Get a pointer to the addvec symbol we just loaded.
  fctn_ptr = dlsym(handle, "addvec");
  if (fctn_ptr == NULL) {
    fprintf(stderr, "%s\n", dlerror());
    dlclose(handle);
    exit(EXIT_FAILURE);
  }

  // Now you can call the addvec() function through fctn_ptr pointer
  fctn_ptr(x, y, z, 2);
  printf("z = [%d %d]\n", z[0], z[1]);

  // Unload the shared library.
  dlclose(handle);
  return 0;
}
