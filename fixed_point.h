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
uint64_t log2fix (struct fixed_64);
static void next_exp();
void print_num(uint64_t);
int64_t logfix (struct fixed_64);
