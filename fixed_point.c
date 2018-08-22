#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>

#include "fixed_point.h"

void set_integral(struct fixed_64* fp, uint64_t num){
  // Get number of integral bits
  uint64_t topbits = (sizeof(uint64_t)*8 - fp->precision);
  // Blow the topbits away
  fp->whole <<= topbits;
  fp->whole >>= topbits;

  fp->whole |= (num << (fp->precision));
}

void set_decimal(struct fixed_64* fp, uint64_t num){
  uint64_t lowerbits = fp->precision;
  uint64_t clearmask = ULLONG_MAX << lowerbits;
  fp->whole &= clearmask;
  
  uint64_t over = ((uint64_t)1u) << fp->precision;
  uint64_t mask = over-1;
  if (fp->precision == (sizeof(uint64_t)*8)){
      mask = ULLONG_MAX;
  }
  fp->whole |= (num & mask);
}

uint64_t get_integral(struct fixed_64 fp){
  return fp.whole >> fp.precision;
}

uint64_t get_decimal(struct fixed_64 fp){
  uint64_t topbits = ((sizeof(uint64_t)*8) - fp.precision);
  return ((fp.whole << topbits) >> topbits);
}

void print(struct fixed_64 fp){
  printf("Whole Number %"PRIx64"\n",fp.whole);
  uint64_t mask = 0xFFFFFFFFFFFFFFFF;
  uint64_t lower = fp.whole & (mask >> fp.precision);
  uint64_t upper = fp.whole >> (fp.precision);
  printf("%" PRIx64 ".%" PRIx64 "\n Prec %"PRIx64"\n",upper, lower, fp.precision);
}

void tests();

int main() {
  
#ifdef TEST
  tests();
#endif
    return 1;  
}

void tests(){
  struct fixed_64 test;
  test.whole = 0;
  test.precision = 32;
  set_integral(&test, 0xFFFFFFFF);
  assert(test.whole == 0xFFFFFFFF00000000);
  assert(get_integral(test) == 0xFFFFFFFF);
  set_decimal(&test, 0xFFFFFFFF);
  assert(test.whole == 0xFFFFFFFFFFFFFFFF);

  test.whole = 0;
  set_decimal(&test, 0xFFFFFFFFFFFFFFFF);
  assert(test.whole == 0x00000000FFFFFFFF);
  assert(get_decimal(test) == 0xFFFFFFFF);
  test.whole = 0;
  test.precision = 0;
  set_integral(&test, 0xFFFFFFFFFFFFFFFF);
  set_decimal(&test, 0x1);
  assert(test.whole == 0xFFFFFFFFFFFFFFFF);
  test.whole = 0;
  test.precision = sizeof(uint64_t)*8;
  set_decimal(&test, 0xFFFFFFFFFFFFFFFF);
  set_integral(&test, 0x1);
  assert(test.whole == 0xFFFFFFFFFFFFFFFF);

  struct fixed_64 a,b,c;
  memset(&a,0,sizeof(struct fixed_64));
  memset(&b,0,sizeof(struct fixed_64));
  memset(&c,0,sizeof(struct fixed_64));

  a.precision = 32;
  set_integral(&a,1);

  assert(a.whole == 0x0000000100000000);

  b.precision = 32;
  set_integral(&b,1);

  assert(b.whole == 0x0000000100000000);

  c = mul_fp(a,b);
  assert(c.whole == 0x0000000100000000);

  set_integral(&a,0x2);
  assert(a.whole == 0x0000000200000000);
  
  c = mul_fp(a,b);
  assert(c.whole == 0x0000000200000000);

  
  
}


struct fixed_64 mul_fp (struct fixed_64 a, struct fixed_64 b) {
  assert(a.precision == b.precision);
  uint64_t a_lower, b_lower, a_upper, b_upper;
  a_upper = get_integral(a);
  a_lower = get_decimal(a);

  b_upper = get_integral(b);
  b_lower = get_decimal(b);

  printf("a:%"PRIx64"\nb:%"PRIx64"\n", a_upper, b_lower);

  uint64_t c_upper = a_upper * b_upper;
  uint64_t c_lower = a_lower * b_lower;

  printf("%"PRIx64"\n%"PRIx64"\n", c_upper, c_lower);

  struct fixed_64 out = {
    .whole = ((c_upper << a.precision) | c_lower),
    .precision = a.precision
  };
  return out;
}
int32_t log2fix (uint32_t x, size_t precision)
{
    int32_t b = 1U << (precision - 1);
    int32_t y = 0;


    if (precision < 1 || precision > 31) {
        return INT32_MAX; // indicates an error
    }

    if (x == 0) {
        return INT32_MIN; // represents negative infinity
    }

    while (x < 1U << precision) {
        x <<= 1;
        y -= 1U << precision;
    }

    while (x >= 2U << precision) {
        x >>= 1;
        y += 1U << precision;
    }

    uint64_t z = x;

    for (size_t i = 0; i < precision; i++) {
        z = z * z >> precision;
        if (z >= 2U << precision) {
            z >>= 1;
            y += b;
        }
        b >>= 1;
    }

    return y;
}
