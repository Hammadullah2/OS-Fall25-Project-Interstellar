#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("FPU Context Switch Test\n");
  printf("========================\n\n");
  
  // Parent starts computation: sum of i^2 from 1.0 to 100.0, step 0.5
  printf("Parent: Starting computation (i^2 from 1.0 to 100.0)...\n");
  
  float parent_sum1 = 0.0;
  float i;
  
  // First half: 1.0 to 50.0
  for(i = 1.0; i <= 50.0; i += 0.5) {
    parent_sum1 += i * i;
  }
  
  printf("Parent: Completed first half, sum = %d (before fork)\n", (int)parent_sum1);
  
  // Fork child process
  int pid = fork();
  
  if(pid < 0) {
    printf("Fork failed\n");
    exit(1);
  }
  
  if(pid == 0) {
    // Child process
    printf("\nChild: Starting computation (i^3 from 1.0 to 50.0)...\n");
    
    float child_sum = 0.0;
    float j;
    
    for(j = 1.0; j <= 50.0; j += 0.5) {
      child_sum += j * j * j;
    }
    
    printf("Child: Completed computation, sum = %d\n", (int)child_sum);
    printf("Child: Expected approximately 1593906\n");
    
    // Check if result is approximately correct (within 1%)
    int expected = 1593906;
    int actual = (int)child_sum;
    int diff = actual > expected ? actual - expected : expected - actual;
    
    if(diff < expected / 100) {
      printf("Child: PASS - Result is correct!\n\n");
    } else {
      printf("Child: FAIL - Result is incorrect (diff = %d)\n\n", diff);
    }
    
    exit(0);
  } else {
    // Parent process continues
    printf("\nParent: Continuing computation (i^2 from 50.5 to 100.0)...\n");
    
    float parent_sum2 = 0.0;
    
    // Second half: 50.5 to 100.0
    for(i = 50.5; i <= 100.0; i += 0.5) {
      parent_sum2 += i * i;
    }
    
    float parent_total = parent_sum1 + parent_sum2;
    
    // Wait for child
    wait(0);
    
    printf("Parent: Completed computation\n");
    printf("Parent: First half sum  = %d\n", (int)parent_sum1);
    printf("Parent: Second half sum = %d\n", (int)parent_sum2);
    printf("Parent: Total sum = %d\n", (int)parent_total);
    printf("Parent: Expected approximately 666866\n");
    
    // Check if result is approximately correct (within 1%)
    int expected = 666866;
    int actual = (int)parent_total;
    int diff = actual > expected ? actual - expected : expected - actual;
    
    if(diff < expected / 100) {
      printf("Parent: PASS - Result is correct!\n");
    } else {
      printf("Parent: FAIL - Result is incorrect (diff = %d)\n", diff);
    }
    
    printf("\n========================\n");
    printf("FPU Test Complete\n");
  }
  
  exit(0);
}
