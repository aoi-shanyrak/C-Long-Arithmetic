#ifndef SAFE_ALLOC_H
#define SAFE_ALLOC_H

#include <stdlib.h>

void* nc_malloc(size_t size);
void* nc_realloc(void* arr, size_t size);

#endif