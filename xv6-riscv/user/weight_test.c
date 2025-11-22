#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char *name = "test_weights";
  char buf[100];
  int len = 100;
  int ret;

  printf("weight_test: starting\n");

  // To load weights
  ret = weight_load(name, buf, len);
  if(ret >= 0){
    printf("weight_test: Weights found Size: %d\n", ret);
    printf("weight_test: Data: %s\n", buf);
    if(strcmp(buf, "Hello, World ") == 0){
      printf("weight_test: Data verification passed \n");
    } else {
      printf("weight_test: Data verification FAILED!\n");
    }
  } else {
    printf("weight_test: Weights not found (ret=%d). Storing \n", ret);
    strcpy(buf, "Hello, World ");
    ret = weight_store(name, buf, strlen(buf) + 1);
    if(ret < 0){
      printf("weight_test: Store failed ret=%d\n", ret);
    } else {
      printf("weight_test: Store successful \n");
    }
  }

  exit(0);
}
