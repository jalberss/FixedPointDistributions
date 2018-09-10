#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "fixed_point.h"

#if DEBUG_OUTPUT                                                                               
#define DEBUG(S, ...) fprintf(stderr, "fpe_preload: debug(%8d): " S, ##__VA_ARGS__)  
#else                                                                                          
#define DEBUG(S, ...)                                                                          
#endif                                                                                         

//#define INV_LOG2_E (uint64_t 0xB17217F7)

struct fixed_64 INV_LOG2_E = {
  .whole = 0x00000000B17217F7,
  .precision = 32,
};

void set_integral(struct fixed_64* fp, uint64_t num)
{
  // Get number of integral bits
  uint64_t topbits = (sizeof(uint64_t)*8 - fp->precision);
  // Blow the topbits away
  fp->whole <<= topbits;
  fp->whole >>= topbits;

  fp->whole |= (num << (fp->precision));
}

void set_decimal(struct fixed_64* fp, uint64_t num)
{
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

uint64_t get_integral(struct fixed_64 fp)
{
  return fp.whole >> fp.precision;
}

uint64_t get_decimal(struct fixed_64 fp)
{
  uint64_t topbits = ((sizeof(uint64_t)*8) - fp.precision);
  return ((fp.whole << topbits) >> topbits);
}

void print(struct fixed_64 fp)
{
  printf("Whole Number %"PRIx64"\n",fp.whole);
  printf("Whole Number %"PRIu64"\n",fp.whole);
  uint64_t mask = 0xFFFFFFFFFFFFFFFF;
  uint64_t lower = fp.whole & (mask >> fp.precision);
  uint64_t upper = fp.whole >> (fp.precision);
  printf("%08" PRIx64 ".%08" PRIx64 "\n Prec %"PRIu64"\n",upper, lower, fp.precision);
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

  test.whole = 0;
  test.precision = 32;
  set_decimal(&test, 0x1);
  assert(test.whole == 0x0000000000000001);

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


  a.precision = 16;
  b.precision = 16;
  a.whole = 0;
  b.whole = 0;
  set_integral(&a,1);
  set_integral(&b,1);
  c = mul_fp(a,b);
  assert(c.whole == 0x0000000000010000);

  set_integral(&a,0x2);
  assert(a.whole == 0x0000000000020000);
  
  c = mul_fp(a,b);
  assert(c.whole == 0x0000000000020000);

  set_integral(&b,2);

  c = mul_fp(a,b);
  assert(c.whole == 0x0000000000040000);


  set_integral(&a,1);
  set_integral(&b,0);  
  set_decimal(&a,0);
  set_decimal(&b,0x8000);
  
  c = mul_fp(a,b);
  assert(c.whole == 0x0000000000008000);
  c = div_fp(a,b);
  assert(c.whole == 0x0000000000020000);
  c = div_fp(b,a);
  assert(c.whole == 0x0000000000008000);
  
  b.whole = 0;
  set_integral(&b,0x4);
  a.whole = 0;
  set_integral(&a,0x2);
  c = div_fp(a,b);
  assert(c.whole == 0x0000000000008000);
  

  set_integral(&a,16);
  assert(log2fix(a)== 0x0000000000040000);
  set_integral(&a,32);
  assert(log2fix(a)== 0x0000000000050000);

  struct fixed_64 x = secs_to_fixed(1, 1, 32);

  print_num(x.whole);
  
  assert(x.whole == 0x0000000100001000);

  x = secs_to_fixed(1, 0x100000, 32);

  assert(x.whole == 0x0000000200000000);
  

  struct fixed_64 rate = {
    .whole = 0x0000000100000000,
    .precision = 32
  };

  while(1){
    next_exp(rate);
  }

}

void print_num(uint64_t a){
  //  return;
  fprintf(stdout,"%016"PRIx64"\n",a);
}

struct fixed_64 div_fp (struct fixed_64 a, struct fixed_64 b)
{
  assert(a.precision == b.precision);
  assert(sizeof(__uint128_t) == 16);
  uint64_t total = ((((__uint128_t)a.whole) << ((__uint128_t)a.precision)) / ((__uint128_t)b.whole));

  struct fixed_64 out = {
    .whole = total,
    .precision = a.precision
  };

  return out;
}

struct fixed_64 mul_fp (struct fixed_64 a, struct fixed_64 b)
{
  assert(a.precision == b.precision);

  __uint128_t total = ((__uint128_t)a.whole) * ((__uint128_t)b.whole);

  total = total >> ((__uint128_t)a.precision); 

  struct fixed_64 out = {
    .whole = (uint64_t)total,
    .precision = a.precision
  };
  return out;
}

// C. S. Turner, "A Fast Binary Logarithm Algorithm", IEEE Signal Processing Mag., pp. 124,140, Sep. 2010.
uint64_t log2fix (struct fixed_64 fp)
{
  uint64_t x= fp.whole;
  uint64_t precision = fp.precision;
  uint64_t b = 1U << (precision - 1);
  uint64_t y = 0;


  if (precision < 1 || precision > 63) {
    return UINT64_MAX; // indicates an error
  }

  if (x == 0) {
    return UINT64_MAX; // represents negative infinity
  }

  uint64_t one = (((uint64_t)(1U)) << precision);
  while (x < one) {
    x <<= 1;
    y -= (((uint64_t)1U) << precision);
  }

  uint64_t two = (((uint64_t)(2U)) << precision);
  while (x >= two) {
    x >>= 1;
    y += (((uint64_t)1U) << precision);
  }

  __uint128_t z = x;

  for (size_t i = 0; i < precision; i++) {
    z = z * z >> precision;
    if (z >=(((uint64_t)(2U)) << precision)) {
      z >>= 1;
      y += b;
    }
    b >>= 1;
  }

  return y;
}



struct fixed_64 secs_to_fixed(time_t seconds, suseconds_t usecs, uint64_t precision){

  // susecs_t is long int, should we check for it being less than 0?

  
  if (usecs < 0){
    fprintf(stderr, "We cannot time travel yet");
    usecs = 0;
  }

  if ((seconds > UINT32_MAX) || (usecs > UINT32_MAX)){
    fprintf(stderr,"WARNING: RESULT WILL OVERFLOW\n");
  }


  seconds <<= precision;

  
  printf("seconds  ");
  print_num(seconds);

  #define USEC_IN_SEC 12
  uint64_t useconds = ((uint64_t)usecs) << USEC_IN_SEC;

  printf("useconds ");
  print_num(useconds);

  uint64_t full = seconds + useconds;

  struct fixed_64 res = {
			 .whole = full,
			 .precision = precision
  };

  return res;
}

static uint64_t next_exp(struct fixed_64 rate_parameter)
{

  uint64_t precision = rate_parameter.precision;
  uint64_t ran = rand();

  ran <<= precision;
  
  struct fixed_64 r = {
    .whole = ran,
    .precision = precision
  };

  uint64_t max = (uint64_t)RAND_MAX;
  max <<= precision;
  struct fixed_64 MAX = {
    .whole = max,
    .precision = precision
  };

  struct fixed_64 c = div_fp(r, MAX);
  struct fixed_64 log = logfix(c);

  struct fixed_64 result = div_fp(log,rate_parameter);
  fprintf(stderr,"Grab this: %"PRIu64"\n", result.whole);
  return result.whole;
}

struct fixed_64 logfix(struct fixed_64 a)
{
  
  uint64_t n_log2 = log2fix(a);
  uint64_t final = ((int64_t)n_log2)*-1;
  

  struct fixed_64 log2 = {
    .whole = final,
    .precision = a.precision,
  };

  struct fixed_64 t = mul_fp(log2,INV_LOG2_E);

  return t;
}
