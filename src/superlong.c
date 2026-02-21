#include "superlong.h"

#include "generate-arr.h"
#include "safe-alloc.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef uint16_t n256plus;
typedef uint32_t n256plusplus;
typedef uint64_t n256plusplusplus;

DEFINE_DYN_ARR(n256, sldigits, 16)

#define SLDIGITS_ARR_PTR(NUM) (&(NUM)->digits)

// initialization

void superlong_init(superlong* num) {
  sldigits_init(SLDIGITS_ARR_PTR(num));
  sldigits_add_tail(SLDIGITS_ARR_PTR(num), 0);
  num->sign = 0;
}

void superlong_deinit(superlong* num) { sldigits_deinit(SLDIGITS_ARR_PTR(num)); }

superlong* new_superlong() {
  superlong* num = nc_malloc(sizeof(superlong));
  superlong_init(num);
  return num;
}

void delete_superlong(superlong* num) {
  if (num) {
    superlong_deinit(num);
    free(num);
  }
}

void superlong_clean(superlong* num) {
  sldigits_deinit(SLDIGITS_ARR_PTR(num));
  sldigits_init(SLDIGITS_ARR_PTR(num));
  num->sign = 0;
}

void superlong_from_uint(superlong* num, uint32_t n) {
  superlong_clean(num);
  if (n == 0) {
    num->sign = 0;
    return;
  }
  while (n > 0) {
    sldigits_add_tail(SLDIGITS_ARR_PTR(num), (n256) (n & 0xFF));
    n >>= 8;
  }
  num->sign = 1;
}

void superlong_from_int(superlong* num, int16_t n) {
  if (n >= 0) {
    superlong_from_uint(num, (uint32_t) n);
  } else {
    superlong_from_uint(num, (uint32_t) (-n));
    num->sign = -1;
  }
}

// side-operations

int superlong_is_zero(const superlong* num) {
  if (num->sign == 0)
    return 1;
  if (num->digits.len == 1 && sldigits_get(SLDIGITS_ARR_PTR(num), 0) == 0)
    return 1;
  return 0;
}

void superlong_normalize(superlong* num) {
  while ((num->digits.len > 1) && (sldigits_get(SLDIGITS_ARR_PTR(num), num->digits.len - 1) == 0)) {
    sldigits_del_tail(SLDIGITS_ARR_PTR(num));
  }
  if (superlong_is_zero(num))
    num->sign = 0;
}

void superlong_negate(superlong* num) {
  if (num->sign != 0)
    num->sign = -num->sign;
}

void superlong_copy(const superlong* num, superlong* res) {
  superlong_clean(res);

  for (size_t i = 0; i < num->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), sldigits_get(SLDIGITS_ARR_PTR(num), i));
  res->sign = num->sign;
}

// absolute value operations

static int superlong_abs_compare_uint(const superlong* a, uint32_t b) {
  if (a->digits.len == 0)
    return (b == 0) ? 0 : -1;
  if (a->digits.len > 4)
    return 1;

  uint32_t val = 0;
  for (size_t i = 0; i < a->digits.len; i++)
    val |= ((uint32_t) sldigits_get(SLDIGITS_ARR_PTR(a), i) << ((i * 8)));

  if (val > b)
    return 1;
  if (val < b)
    return -1;
  return 0;
}

static int superlong_abs_compare(const superlong* a, const superlong* b) {
  if (a->digits.len != b->digits.len)
    return (a->digits.len > b->digits.len) ? 1 : -1;

  for (size_t i = a->digits.len; i > 0; i--) {
    size_t idx = i - 1;
    n256 da = sldigits_get(SLDIGITS_ARR_PTR(a), idx);
    n256 db = sldigits_get(SLDIGITS_ARR_PTR(b), idx);

    if (da != db)
      return (da > db) ? 1 : -1;
  }
  return 0;
}

static void superlong_abs_add_uint(const superlong* a, uint32_t b, superlong* res) {
  superlong_clean(res);

  n256plusplusplus carry = b;
  for (size_t i = 0; i < a->digits.len; i++) {
    carry += (n256plusplusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i);
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (carry & 0xFF));
    carry >>= 8;
  }
  while (carry) {
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (carry & 0xFF));
    carry >>= 8;
  }
  superlong_normalize(res);
}

static void superlong_abs_add(const superlong* a, const superlong* b, superlong* res) {
  // Handle pointer aliasing
  if (a == res || b == res) {
    superlong temp_a, temp_b;
    superlong_init(&temp_a);
    superlong_init(&temp_b);
    superlong_copy(a, &temp_a);
    superlong_copy(b, &temp_b);
    superlong_abs_add(&temp_a, &temp_b, res);
    superlong_deinit(&temp_a);
    superlong_deinit(&temp_b);
    return;
  }
  
  size_t max_len = (a->digits.len > b->digits.len) ? a->digits.len : b->digits.len;
  superlong_clean(res);

  n256plus carry = 0;
  for (size_t i = 0; i < max_len; i++) {
    n256plus sum = carry;
    sum += (n256plus) sldigits_get_or(SLDIGITS_ARR_PTR(a), i, 0);
    sum += (n256plus) sldigits_get_or(SLDIGITS_ARR_PTR(b), i, 0);

    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (sum & 0xFF));
    carry = sum >> 8;
  }
  if (carry > 0)
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) carry);
  superlong_normalize(res);
}

static void superlong_abs_sub_uint(const superlong* a, uint32_t b, superlong* res) {
  superlong_clean(res);

  int32_t borrow = 0;
  n256plusplus bval = b;
  for (size_t i = 0; i < a->digits.len; i++) {
    n256plusplus digit = (n256plusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i);
    n256plusplus sub = (i < 4) ? ((bval >> (i * 8)) & 0xFF) : 0;

    int32_t diff = (int32_t) digit - (int32_t) sub - borrow;
    if (diff < 0) {
      diff += 256;
      borrow = 1;
    } else
      borrow = 0;

    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (diff & 0xFF));
  }
  superlong_normalize(res);
}

static void superlong_abs_uint_sub(uint32_t b, const superlong* a, superlong* res) {
  superlong_clean(res);

  int32_t borrow = 0;
  n256plusplus bval = b;
  for (size_t i = 0; i < a->digits.len; i++) {
    n256plusplus a_digit = (n256plusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i);
    n256plusplus b_digit = (i < 4) ? ((bval >> (i * 8)) & 0xFF) : 0;

    int32_t diff = (int32_t) b_digit - (int32_t) a_digit - borrow;
    if (diff < 0) {
      diff += 256;
      borrow = 1;
    } else
      borrow = 0;

    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (diff & 0xFF));
  }
  superlong_normalize(res);
}

static void superlong_abs_sub(const superlong* a, const superlong* b, superlong* res) {
  // Handle pointer aliasing
  if (a == res || b == res) {
    superlong temp_a, temp_b;
    superlong_init(&temp_a);
    superlong_init(&temp_b);
    superlong_copy(a, &temp_a);
    superlong_copy(b, &temp_b);
    superlong_abs_sub(&temp_a, &temp_b, res);
    superlong_deinit(&temp_a);
    superlong_deinit(&temp_b);
    return;
  }
  
  superlong_clean(res);

  int borrow = 0;
  for (size_t i = 0; i < a->digits.len; i++) {
    int da = (int) sldigits_get(SLDIGITS_ARR_PTR(a), i);
    int db = (int) sldigits_get_or(SLDIGITS_ARR_PTR(b), i, 0);

    int diff = da - db - borrow;
    if (diff < 0) {
      diff += 256;
      borrow = 1;
    } else
      borrow = 0;

    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) diff);
  }
  superlong_normalize(res);
}

// operations

void superlong_add_uint(const superlong* a, uint32_t b, superlong* res) {
  if (a->sign == 0) {
    superlong_from_uint(res, b);
    return;
  }
  if (b == 0) {
    superlong_copy(a, res);
    return;
  }
  if (a->sign > 0) {
    superlong_abs_add_uint(a, b, res);
    res->sign = 1;
  } else {
    int cmp = superlong_abs_compare_uint(a, b);

    if (cmp == 0) {
      superlong_clean(res);
      res->sign = 0;
    } else if (cmp > 0) {
      superlong_abs_sub_uint(a, b, res);
      res->sign = -1;
    } else {
      superlong_abs_uint_sub(b, a, res);
      res->sign = 1;
    }
  }
  superlong_normalize(res);
}

void superlong_add(const superlong* a, const superlong* b, superlong* res) {
  if (a->sign == 0) {
    superlong_copy(b, res);
    return;
  }
  if (b->sign == 0) {
    superlong_copy(a, res);
    return;
  }
  if (a->sign == b->sign) {
    superlong_abs_add(a, b, res);
    res->sign = a->sign;
  } else {
    int cmp = superlong_abs_compare(a, b);

    if (cmp == 0) {
      superlong_clean(res);
      res->sign = 0;
    } else if (cmp > 0) {
      superlong_abs_sub(a, b, res);
      res->sign = a->sign;
    } else {
      superlong_abs_sub(b, a, res);
      res->sign = b->sign;
    }
  }
  superlong_normalize(res);
}

void superlong_sub_uint(const superlong* a, uint32_t b, superlong* res) {
  // Handle case where a == res by using a temporary
  if (a == res) {
    superlong temp;
    superlong_init(&temp);
    superlong_copy(a, &temp);
    superlong_sub_uint(&temp, b, res);
    superlong_deinit(&temp);
    return;
  }
  
  if (a->sign == 0) {
    superlong_from_uint(res, b);
    res->sign = -1;
    return;
  }
  if (b == 0) {
    superlong_copy(a, res);
    return;
  }
  if (a->sign > 0) {
    int cmp = superlong_abs_compare_uint(a, b);

    if (cmp == 0) {
      superlong_clean(res);
      res->sign = 0;
    } else if (cmp > 0) {
      superlong_abs_sub_uint(a, b, res);
      res->sign = 1;
    } else {
      superlong_abs_uint_sub(b, a, res);
      res->sign = -1;
    }
  } else {
    superlong_abs_add_uint(a, b, res);
    res->sign = -1;
  }
  superlong_normalize(res);
}

void superlong_sub(const superlong* a, const superlong* b, superlong* res) {
  superlong neg_b;
  superlong_init(&neg_b);
  superlong_copy(b, &neg_b);

  if (neg_b.sign != 0)
    neg_b.sign = -neg_b.sign;

  superlong_add(a, &neg_b, res);
  superlong_deinit(&neg_b);
}

void superlong_mul_uint(const superlong* a, uint32_t b, superlong* res) {
  if (a->sign == 0 || b == 0) {
    superlong_clean(res);
    res->sign = 0;
    return;
  }
  if (b == 1) {
    if (a != res)
      superlong_copy(a, res);
    return;
  }
  
  // Handle case where a == res by using a temporary
  if (a == res) {
    superlong temp;
    superlong_init(&temp);
    superlong_copy(a, &temp);
    superlong_mul_uint(&temp, b, res);
    superlong_deinit(&temp);
    return;
  }
  
  superlong_clean(res);
  
  if (b == 256) {
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), 0);
    for (size_t i = 0; i < a->digits.len; i++) {
      sldigits_add_tail(SLDIGITS_ARR_PTR(res), sldigits_get(SLDIGITS_ARR_PTR(a), i));
    }
    res->sign = a->sign;
    superlong_normalize(res);
    return;
  }
  n256plusplusplus carry = 0;
  for (size_t i = 0; i < a->digits.len; i++) {
    n256plusplusplus product = (n256plusplusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i) * (n256plusplusplus) b + carry;
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (product & 0xFF));
    carry = product >> 8;
  }
  while (carry > 0) {
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), (n256) (carry & 0xFF));
    carry >>= 8;
  }
  res->sign = a->sign;
  superlong_normalize(res);
}

static void superlong_shift_left_bytes(superlong* num, size_t bytes) {
  if (bytes == 0 || superlong_is_zero(num))
    return;

  superlong temp;
  superlong_init(&temp);

  for (size_t i = 0; i < bytes; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&temp), 0);

  for (size_t i = 0; i < num->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&temp), sldigits_get(SLDIGITS_ARR_PTR(num), i));

  temp.sign = num->sign;
  superlong_deinit(num);
  *num = temp;
}

static void superlong_mul_simple(const superlong* a, const superlong* b, superlong* res) {
  superlong_clean(res);
  superlong temp_res;
  superlong_init(&temp_res);
  superlong_from_uint(&temp_res, 0); // Initialize as zero properly

  for (size_t i = 0; i < a->digits.len; i++) {
    superlong temp;
    superlong_init(&temp);
    superlong_clean(&temp); // Clean to get empty array

    for (size_t k = 0; k < i; k++)
      sldigits_add_tail(SLDIGITS_ARR_PTR(&temp), 0);

    n256plusplus carry = 0;
    for (size_t j = 0; j < b->digits.len; j++) {
      n256plusplus a_digit = (n256plusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i);
      n256plusplus b_digit = (n256plusplus) sldigits_get(SLDIGITS_ARR_PTR(b), j);
      n256plusplus product = (n256plusplus) (a_digit * b_digit) + carry;
      sldigits_add_tail(SLDIGITS_ARR_PTR(&temp), (n256) (product & 0xFF));
      carry = product >> 8;
    }
    while (carry > 0) {
      sldigits_add_tail(SLDIGITS_ARR_PTR(&temp), (n256) (carry & 0xFF));
      carry >>= 8;
    }
    
    temp.sign = 1; // Set sign after adding digits

    superlong new_temp_res;
    superlong_init(&new_temp_res);
    superlong_add(&temp_res, &temp, &new_temp_res);

    superlong_deinit(&temp_res);
    superlong_deinit(&temp);
    temp_res = new_temp_res;
  }
  superlong_deinit(res);
  *res = temp_res;
  superlong_normalize(res);
}

static void superlong_mul_karatsuba(const superlong* x, const superlong* y, superlong* res) {
  size_t min_len = (x->digits.len < y->digits.len) ? x->digits.len : y->digits.len;

  if (min_len < 16) {
    superlong_mul_simple(x, y, res);
    return;
  }
  size_t k = min_len / 2;

  superlong a, b;
  superlong_init(&a);
  superlong_init(&b);

  for (size_t i = 0; i < k && i < x->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&b), sldigits_get(SLDIGITS_ARR_PTR(x), i));

  for (size_t i = k; i < x->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&a), sldigits_get(SLDIGITS_ARR_PTR(x), i));

  a.sign = 1;
  b.sign = 1;

  superlong c, d;
  superlong_init(&c);
  superlong_init(&d);

  for (size_t i = 0; i < k && i < y->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&d), sldigits_get(SLDIGITS_ARR_PTR(y), i));

  for (size_t i = k; i < y->digits.len; i++)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&c), sldigits_get(SLDIGITS_ARR_PTR(y), i));

  c.sign = 1;
  d.sign = 1;

  superlong ac;
  superlong_init(&ac);
  superlong_mul_karatsuba(&a, &c, &ac);

  superlong bd;
  superlong_init(&bd);
  superlong_mul_karatsuba(&b, &d, &bd);

  superlong a_b, c_d;
  superlong_init(&a_b);
  superlong_init(&c_d);
  superlong_add(&a, &b, &a_b);
  superlong_add(&c, &d, &c_d);

  superlong ad_bc_ac_bd;
  superlong_init(&ad_bc_ac_bd);
  superlong_mul_karatsuba(&a_b, &c_d, &ad_bc_ac_bd);

  superlong temp;
  superlong_init(&temp);
  superlong_sub(&ad_bc_ac_bd, &ac, &temp);
  superlong_deinit(&ad_bc_ac_bd);
  superlong_init(&ad_bc_ac_bd);
  superlong_sub(&temp, &bd, &ad_bc_ac_bd);
  superlong_deinit(&temp);

  superlong_shift_left_bytes(&ac, 2 * k);
  superlong_shift_left_bytes(&ad_bc_ac_bd, k);

  superlong_init(&temp);
  superlong_add(&ac, &ad_bc_ac_bd, &temp);

  superlong final;
  superlong_init(&final);
  superlong_add(&temp, &bd, &final);

  superlong_deinit(res);
  *res = final;

  superlong_deinit(&a);
  superlong_deinit(&b);
  superlong_deinit(&c);
  superlong_deinit(&d);
  superlong_deinit(&ac);
  superlong_deinit(&bd);
  superlong_deinit(&ad_bc_ac_bd);
  superlong_deinit(&a_b);
  superlong_deinit(&c_d);
  superlong_deinit(&temp);
}

void superlong_mul(const superlong* a, const superlong* b, superlong* res) {
  if ((a->sign == 0) || (b->sign == 0)) {
    superlong_clean(res);
    res->sign = 0;
    return;
  }
  superlong_mul_karatsuba(a, b, res);
  res->sign = (a->sign == b->sign) ? 1 : -1;
  superlong_normalize(res);
}

static n256 bin_find_digit(const superlong* remainder, const superlong* divisor) {
  n256 left = 1, right = 255, best = 0;
  while (left <= right) {
    n256 mid = (n256) (left + (right - left) / 2);

    superlong prod;
    superlong_init(&prod);
    superlong_mul_uint(divisor, mid, &prod);

    int cmp = superlong_abs_compare(&prod, remainder);
    if (cmp <= 0) {
      best = mid;
      left = mid + 1;
    } else
      right = mid - 1;

    superlong_deinit(&prod);
  }
  return best;
}

void superlong_div_uint(const superlong* a, uint32_t b, superlong* res) {
  if (b == 0) {
    perror("Division by zero\n");
    exit(1);
  }
  
  // Handle case where a == res by using a temporary
  if (a == res) {
    superlong temp;
    superlong_init(&temp);
    superlong_copy(a, &temp);
    superlong_div_uint(&temp, b, res);
    superlong_deinit(&temp);
    return;
  }
  
  if (a->sign == 0) {
    superlong_clean(res);
    res->sign = 0;
    return;
  }
  if (b == 1) {
    superlong_copy(a, res);
    return;
  }
  if (b == 256) {
    superlong_clean(res);
    if (a->digits.len > 1) {
      for (size_t i = 1; i < a->digits.len; i++) {
        sldigits_add_tail(SLDIGITS_ARR_PTR(res), sldigits_get(SLDIGITS_ARR_PTR(a), i));
      }
    }
    res->sign = a->sign;
    superlong_normalize(res);
    return;
  }
  superlong_clean(res);

  n256plusplusplus remaind = 0;
  n256* q_digits = nc_malloc(a->digits.len * sizeof(n256));
  size_t q_len = 0;
  for (size_t i = a->digits.len; i-- > 0;) {
    n256plusplusplus cur = ((n256plusplusplus) sldigits_get(SLDIGITS_ARR_PTR(a), i) + remaind * 256);
    n256plusplusplus q_digit = cur / b;
    remaind = cur % b;
    q_digits[q_len++] = (n256) q_digit;
  }
  // from big-endian to little-endian
  for (size_t i = q_len; i > 0; i--)
    sldigits_add_tail(SLDIGITS_ARR_PTR(res), q_digits[i - 1]);
  free(q_digits);
  res->sign = a->sign;
  superlong_normalize(res);
}

void superlong_div(const superlong* a, const superlong* b, superlong* res) {
  if (b->sign == 0) {
    perror("Division by zero\n");
    exit(1);
  }
  if (a->sign == 0) {
    superlong_clean(res);
    res->sign = 0;
    return;
  }
  int sign = (a->sign == b->sign) ? 1 : -1;

  // Optimization: if divisor fits in uint32_t, use div_uint
  if (b->digits.len <= 4) {
    uint32_t b_val = 0;
    for (size_t i = 0; i < b->digits.len; i++) {
      b_val |= ((uint32_t)sldigits_get(SLDIGITS_ARR_PTR(b), i) << (i * 8));
    }
    if (b_val != 0) {
      superlong_div_uint(a, b_val, res);
      if (sign < 0 && res->sign != 0) {
        res->sign = -1;
      }
      return;
    }
  }

  superlong divid, divis;
  superlong_init(&divid);
  superlong_init(&divis);
  superlong_copy(a, &divid);
  superlong_copy(b, &divis);
  divid.sign = 1;
  divis.sign = 1;

  // if dividend < divisor then result is 0 and remainder is dividend
  if (superlong_abs_compare(&divid, &divis) < 0) {
    superlong_clean(res);
    res->sign = 0;
    superlong_deinit(&divid);
    superlong_deinit(&divis);
    return;
  }
  superlong remaind;
  superlong quo;
  superlong_init(&remaind);
  superlong_init(&quo);
  superlong_clean(&quo); // Clean to get empty array for adding digits
  superlong_from_uint(&remaind, 0);

  n256* q_digits = nc_malloc(divid.digits.len * sizeof(n256));
  size_t q_len = 0;

  for (size_t i = divid.digits.len; i-- > 0;) {
    superlong tmp;
    superlong byte_val;
    superlong new_rem;

    superlong_init(&tmp);
    superlong_init(&byte_val);
    superlong_init(&new_rem);

    superlong_mul_uint(&remaind, 256, &tmp); // getting next byte in remainder

    uint8_t current_byte = sldigits_get(SLDIGITS_ARR_PTR(&divid), i);
    superlong_from_uint(&byte_val, current_byte);

    superlong_add(&tmp, &byte_val, &new_rem);

    superlong_deinit(&tmp);
    superlong_deinit(&byte_val);
    superlong_deinit(&remaind);

    remaind = new_rem;
    n256 q_digit = 0;
    if (superlong_abs_compare(&remaind, &divis) >= 0) {
      q_digit = bin_find_digit(&remaind, &divis);

      superlong product;
      superlong new_rem2;
      superlong_init(&product);
      superlong_init(&new_rem2);
      superlong_mul_uint(&divis, q_digit, &product);

      superlong_abs_sub(&remaind, &product, &new_rem2);

      superlong_deinit(&remaind);
      remaind = new_rem2;
      superlong_deinit(&product);
    }
    q_digits[q_len++] = q_digit;
  }
  // from big-endian to little-endian
  for (size_t i = q_len; i > 0; i--)
    sldigits_add_tail(SLDIGITS_ARR_PTR(&quo), q_digits[i - 1]);

  free(q_digits);

  quo.sign = sign; // Set sign before normalize
  superlong_normalize(&quo);

  superlong_deinit(res);
  *res = quo;
  quo.digits.arr = NULL;

  superlong_deinit(&divid);
  superlong_deinit(&divis);
  superlong_deinit(&remaind);
}

void superlong_factorial(uint32_t n, superlong* res) {
  superlong_from_uint(res, 1);

  superlong temp;
  superlong_init(&temp);

  for (uint32_t i = 2; i <= n; i++) {
    superlong_mul_uint(res, i, &temp);
    superlong_copy(&temp, res);
  }
  superlong_deinit(&temp);
}

static n256plus superlong_div_uint10(superlong* num) {
  n256plus remainder = 0;

  for (size_t i = num->digits.len; i > 0; i--) {
    size_t idx = i - 1;
    n256plus current = (n256plus) ((n256plus) sldigits_get(SLDIGITS_ARR_PTR(num), idx) + remainder * (n256plus) 256);
    n256plus new_digit = (n256plus) (current / (n256plus) 10);
    remainder = (n256plus) (current % (n256plus) 10);
    sldigits_set(SLDIGITS_ARR_PTR(num), idx, (n256) new_digit);
  }
  superlong_normalize(num);
  return remainder;
}

char* superlong_to_decimal_str(const superlong* num) {
  if (superlong_is_zero(num)) {
    char* result = nc_malloc(2);
    result[0] = '0';
    result[1] = '\0';
    return result;
  }

  superlong temp;
  superlong_init(&temp);
  superlong_copy(num, &temp);
  temp.sign = 1;

  size_t max_digits = temp.digits.len * 3 + 10;
  char* digits = nc_malloc(max_digits);

  size_t digit_count = 0;
  while (!superlong_is_zero(&temp)) {
    n256plus rem = superlong_div_uint10(&temp);
    digits[digit_count++] = '0' + (char) rem;
  }

  size_t result_len = digit_count + (num->sign < 0 ? 1 : 0) + 1;
  char* result = nc_malloc(result_len);
  size_t pos = 0;

  if (num->sign < 0)
    result[pos++] = '-';

  for (size_t i = digit_count; i > 0; i--)
    result[pos++] = digits[i - 1];

  result[pos] = '\0';

  free(digits);
  superlong_deinit(&temp);
  return result;
}
