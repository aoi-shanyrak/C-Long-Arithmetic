#ifndef GENERATE_ARR_H
#define GENERATE_ARR_H

#include "safe-alloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECLARE_DYN_ARR(T, NAME, DEFAULT_CAP)                                                                          \
  typedef struct {                                                                                                     \
    T* arr;                                                                                                            \
    size_t len;                                                                                                        \
    size_t cap;                                                                                                        \
  } NAME;                                                                                                              \
                                                                                                                       \
  void NAME##_init(NAME* arr);                                                                                         \
  void NAME##_deinit(NAME* arr);                                                                                       \
  NAME* new_##NAME();                                                                                                  \
  void delete_##NAME(NAME* arr);                                                                                       \
  void NAME##_ensure_capacity(NAME* arr, size_t required_cap);                                                         \
                                                                                                                       \
  void NAME##_add_tail(NAME* arr, T elem);                                                                             \
  void NAME##_fill(NAME* arr, size_t count, T filler);                                                                 \
  T NAME##_del_tail(NAME* arr);                                                                                        \
  T NAME##_get(const NAME* arr, size_t index);                                                                         \
  T NAME##_get_or(const NAME* arr, size_t index, T _default);                                                          \
  void NAME##_set(NAME* arr, size_t index, T elem);                                                                    \
  T NAME##_del(NAME* arr, size_t index);


#define DEFINE_DYN_ARR(T, NAME, DEFAULT_CAP)                                                                           \
                                                                                                                       \
  void NAME##_init(NAME* arr) {                                                                                        \
    arr->arr = nc_malloc(DEFAULT_CAP * sizeof(T));                                                                     \
    arr->len = 0;                                                                                                      \
    arr->cap = DEFAULT_CAP;                                                                                            \
  }                                                                                                                    \
                                                                                                                       \
  void NAME##_deinit(NAME* arr) {                                                                                      \
    free(arr->arr);                                                                                                    \
    arr->arr = NULL;                                                                                                   \
    arr->len = 0;                                                                                                      \
    arr->cap = 0;                                                                                                      \
  }                                                                                                                    \
                                                                                                                       \
  NAME* new_##NAME() {                                                                                                 \
    NAME* arr = nc_malloc(sizeof(NAME));                                                                               \
    NAME##_init(arr);                                                                                                  \
    return arr;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  void delete_##NAME(NAME* arr) {                                                                                      \
    NAME##_deinit(arr);                                                                                                \
    free(arr);                                                                                                         \
  }                                                                                                                    \
                                                                                                                       \
  void NAME##_ensure_capacity(NAME* arr, size_t required_cap) {                                                        \
    while (arr->cap < required_cap)                                                                                    \
      arr->cap *= 2;                                                                                                   \
    arr->arr = nc_realloc(arr->arr, arr->cap * sizeof(T));                                                             \
  }                                                                                                                    \
                                                                                                                       \
  void NAME##_add_tail(NAME* arr, T elem) {                                                                            \
    NAME##_ensure_capacity(arr, arr->len + 1);                                                                         \
    arr->arr[arr->len] = elem;                                                                                         \
    arr->len++;                                                                                                        \
  }                                                                                                                    \
                                                                                                                       \
  void NAME##_fill(NAME* arr, size_t count, T filler) {                                                                \
    for (size_t i = 0; i < count; i++) {                                                                               \
      NAME##_add_tail(arr, filler);                                                                                    \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  T NAME##_del_tail(NAME* arr) { return arr->arr[--(arr->len)]; }                                                      \
                                                                                                                       \
  T NAME##_get(const NAME* arr, size_t index) { return arr->arr[index]; }                                              \
                                                                                                                       \
  T NAME##_get_or(const NAME* arr, size_t index, T _default) {                                                         \
    return (index < arr->len) ? NAME##_get(arr, index) : _default;                                                     \
  }                                                                                                                    \
                                                                                                                       \
  void NAME##_set(NAME* arr, size_t index, T elem) { arr->arr[index] = elem; }                                         \
                                                                                                                       \
  T NAME##_del(NAME* arr, size_t index) {                                                                              \
    T removed = arr->arr[index];                                                                                       \
    memmove(&arr->arr[index], &arr->arr[index + 1], (arr->len - index - 1) * sizeof(T));                               \
    arr->len--;                                                                                                        \
    return removed;                                                                                                    \
  }

#endif
