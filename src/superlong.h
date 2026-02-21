#ifndef SUPERLONGINT_H
#define SUPERLONGINT_H

#include "generate-arr.h"

#include <stdint.h>

typedef uint8_t n256;

DECLARE_DYN_ARR(n256, sldigits, 16)

typedef struct {
  sldigits digits;
  int sign;
} superlong;

// initialization
void superlong_init(superlong*);
void superlong_deinit(superlong*);
superlong* new_superlong();
void delete_superlong(superlong*);
void superlong_from_int(superlong*, int16_t n);
void superlong_from_uint(superlong*, uint32_t n);

// operations
void superlong_add(const superlong*, const superlong*, superlong* res);
void superlong_add_uint(const superlong*, uint32_t, superlong* res);

void superlong_sub(const superlong*, const superlong*, superlong* res);
void superlong_sub_uint(const superlong*, uint32_t, superlong* res);

void superlong_mul(const superlong*, const superlong*, superlong* res);
void superlong_mul_uint(const superlong*, uint32_t, superlong* res);

void superlong_div(const superlong*, const superlong*, superlong* res);
void superlong_div_uint(const superlong*, uint32_t, superlong* res);

// other operations
void superlong_copy(const superlong*, superlong* res);

void superlong_negate(superlong*);
int superlong_is_zero(const superlong*);

char* superlong_to_decimal_str(const superlong*);

void superlong_factorial(uint32_t, superlong* res);

#endif
