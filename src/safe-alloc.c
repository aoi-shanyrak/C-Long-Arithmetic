#include "safe-alloc.h"

#include <stdio.h>
#include <stdlib.h>

void* nc_malloc(size_t size) {
  void* result = malloc(size);
  if (result == NULL) {
    printf("Out of memory\n");
    exit(1);
  }
  return result;
}

void* nc_realloc(void* arr, size_t size) {
  void* result = realloc(arr, size);
  if (result == NULL) {
    printf("Out of memory\n");
    exit(1);
  }
  return result;
}
