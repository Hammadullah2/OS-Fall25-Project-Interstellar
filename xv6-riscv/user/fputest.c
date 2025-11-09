// xv6-riscv/user/fputest.c
#include "kernel/types.h"
#include "user/user.h"

void print_float(float val) {
  int intp = (int)val;
  int frac = (int)((val - intp) * 1000);
  if (frac < 0) frac = -frac;
  if (frac < 10) printf("%d.00%d", intp, frac);
  else if (frac < 100) printf("%d.0%d", intp, frac);
  else printf("%d.%d", intp, frac);
}

int
main(int argc, char *argv[])
{
  volatile float sum = 0.0f;
  float i;

  // parent computes first half: 1.0 .. 50.0 step 0.5
  for (i = 1.0f; i <= 50.0f; i += 0.5f)
    sum += i * i;

  int pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  } else if (pid == 0) {
    // child computes cubes 1.0 .. 50.0 step 0.5
    volatile float cubes = 0.0f;
    float j;
    for (j = 1.0f; j <= 50.0f; j += 0.5f)
      cubes += j * j * j;
    printf("Child sum of cubes = ");
    print_float(cubes);
    printf("\n");
    exit(0);
  } else {
    wait(0);
    // parent continues: 50.5 .. 100.0 step 0.5
    for (i = 50.5f; i <= 100.0f; i += 0.5f)
      sum += i * i;
    printf("Parent total sum of squares = ");
    print_float(sum);
    printf("\n");
    exit(0);
  }
}
