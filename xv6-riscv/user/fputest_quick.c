#include "kernel/types.h"
#include "user/user.h"

void print_float(float val) {
  int int_part = (int)val;
  int frac_part = (int)((val - int_part) * 1000);
  if (frac_part < 0) frac_part = -frac_part; // handle negatives

  // manual padding for 3 decimal digits
  if (frac_part < 10)
    printf("%d.00%d", int_part, frac_part);
  else if (frac_part < 100)
    printf("%d.0%d", int_part, frac_part);
  else
    printf("%d.%d", int_part, frac_part);
}

int
main(int argc, char *argv[])
{
  volatile float a = 1.5f;
  volatile float b = 2.25f;
  volatile float c = a * b + 1.0f;

  printf("fputest_quick:\n");
  printf("a = "); print_float(a); printf("\n");
  printf("b = "); print_float(b); printf("\n");
  printf("c = "); print_float(c); printf("\n");

  exit(0);
}
