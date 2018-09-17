struct fixed_64 {
  uint64_t whole;
  uint64_t precision; // no int char comparisons, number of decimal places
};


struct fixed_64 mul_fp (struct fixed_64, struct fixed_64);
struct fixed_64 div_fp (struct fixed_64, struct fixed_64);

void set_integral(struct fixed_64*, uint64_t);
void set_decimal(struct fixed_64*, uint64_t);
uint64_t get_integral(struct fixed_64);
uint64_t get_decimal(struct fixed_64);
void print(struct fixed_64);


static struct fixed_64 next_exp(struct fixed_64 parameter);
void print_num(uint64_t);


struct fixed_64 get_next_exp(time_t seconds, suseconds_t usecs);
struct fixed_64 logfix (struct fixed_64);
uint64_t log2fix (struct fixed_64);



struct fixed_64 secs_to_fixed(time_t, suseconds_t, uint64_t);
suseconds_t get_usecs(struct fixed_64 fp);
time_t get_secs(struct fixed_64 fp);
