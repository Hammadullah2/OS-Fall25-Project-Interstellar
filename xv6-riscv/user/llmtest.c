#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void*
load_model(char *filename, int *model_size)
{
  int size;
  void *mem;
  int fd;
  struct stat st;

  printf("LLM: Checking cache for %s \n", filename);
  
  // 1. Check cache
  size = weight_load(filename, 0, 0);
  
  if(size > 0){
    printf("LLM: Cache hit Size: %d\n", size);
    mem = malloc(size);
    if(mem == 0){
      printf("LLM: malloc failed\n");
      return 0;
    }
    if(weight_load(filename, mem, size) != size){
      printf("LLM: Cache load failed\n");
      free(mem);
      return 0;
    }
    *model_size = size;
    return mem;
  }

  printf("LLM: Cache miss. Loading from file %s \n", filename);

  // 2. Load from file
  fd = open(filename, O_RDONLY);
  if(fd < 0){
    printf("LLM: Could not open file %s\n", filename);
    return 0;
  }

  if(fstat(fd, &st) < 0){
    printf("LLM: fstat failed\n");
    close(fd);
    return 0;
  }

  size = st.size;
  mem = malloc(size);
  if(mem == 0){
    printf("LLM: malloc failed\n");
    close(fd);
    return 0;
  }

  if(read(fd, mem, size) != size){
    printf("LLM: read failed\n");
    free(mem);
    close(fd);
    return 0;
  }
  close(fd);

  // 3. Store in cache
  printf("LLM: Storing in cache \n");
  if(weight_store(filename, mem, size) < 0){
    printf("LLM: Cache store failed (maybe full?)\n");
  } else {
    printf("LLM: Stored successfully.\n");
  }

  *model_size = size;
  return mem;
}

int
main(int argc, char *argv[])
{
  char *filename = "README"; // Use README as a dummy model file
  int size;
  void *data;

  printf("LLM Integration Test\n");

  // Loads from file and cache (or cache hit if run previously)
  data = load_model(filename, &size);
  if(data){
    printf("Loaded %d bytes. First few chars: %c%c%c\n", size, ((char*)data)[0], ((char*)data)[1], ((char*)data)[2]);
  }

  exit(0);
}
