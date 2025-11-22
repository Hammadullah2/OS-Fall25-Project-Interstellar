#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct page_node {
  void *page;
  struct page_node *next;
};

struct weight_buffer {
  char name[64];
  struct page_node *pages; // Linked list of pages
  int size;
  struct weight_buffer *next;
};

struct {
  struct spinlock lock;
  struct weight_buffer *head;
} weight_store;

void
weightinit(void)
{
  initlock(&weight_store.lock, "weight_store");
  weight_store.head = 0;
}

// Helper to free a weight buffer
void
free_weight_buffer(struct weight_buffer *wb)
{
  struct page_node *curr = wb->pages;
  while(curr){
    struct page_node *next = curr->next;
    if(curr->page) kfree(curr->page);
    kfree((void*)curr);
    curr = next;
  }
  kfree((void*)wb);
}

int
weight_store_add(char *name, uint64 user_src, int len)
{
  struct weight_buffer *wb;
  
  acquire(&weight_store.lock);
  
  // Check if already exists
  for(wb = weight_store.head; wb != 0; wb = wb->next){
    if(strncmp(wb->name, name, 64) == 0){
      release(&weight_store.lock);
      return -1; //if exists
    }
  }

  // Allocate new buffer struct
  wb = (struct weight_buffer*)kalloc();
  if(wb == 0){
    release(&weight_store.lock);
    return -1;
  }
  memset(wb, 0, PGSIZE); // kalloc returns a page, use it as struct
  
  safestrcpy(wb->name, name, 64);
  wb->size = len;
  wb->pages = 0;

  // Copy data from user space
  int remaining = len;
  uint64 src_addr = user_src;
  struct page_node **tail = &wb->pages;

  while(remaining > 0){
    struct page_node *node = (struct page_node*)kalloc();
    if(node == 0){
      free_weight_buffer(wb);
      release(&weight_store.lock);
      return -1;
    }
    memset(node, 0, PGSIZE);
    
    node->page = kalloc();
    if(node->page == 0){
      kfree((void*)node);
      free_weight_buffer(wb);
      release(&weight_store.lock);
      return -1;
    }
    
    int n = remaining > PGSIZE ? PGSIZE : remaining;
    struct proc *p = myproc();
    if(copyin(p->pagetable, (char*)node->page, src_addr, n) < 0){
      kfree(node->page);
      kfree((void*)node);
      free_weight_buffer(wb);
      release(&weight_store.lock);
      return -1;
    }

    *tail = node;
    tail = &node->next;
    
    src_addr += n;
    remaining -= n;
  }

  wb->next = weight_store.head;
  weight_store.head = wb;
  
  release(&weight_store.lock);
  return 0;
}

int
weight_store_get(char *name, uint64 user_dst, int len)
{
  struct weight_buffer *wb;
  
  acquire(&weight_store.lock);
  
  for(wb = weight_store.head; wb != 0; wb = wb->next){
    if(strncmp(wb->name, name, 64) == 0){
      break;
    }
  }
  
  if(wb == 0){
    release(&weight_store.lock);
    return -1;
  }

  // Allow querying size
  if(user_dst == 0 || len == 0){
    int size = wb->size;
    release(&weight_store.lock);
    return size;
  }

  if(len < wb->size){
    release(&weight_store.lock);
    return -2; // Buffer too small
  }

  int remaining = wb->size;
  uint64 dst_addr = user_dst;
  struct page_node *curr = wb->pages;

  while(remaining > 0 && curr){
    int n = remaining > PGSIZE ? PGSIZE : remaining;
    struct proc *p = myproc();
    if(copyout(p->pagetable, dst_addr, (char*)curr->page, n) < 0){
      release(&weight_store.lock);
      return -1;
    }
    dst_addr += n;
    remaining -= n;
    curr = curr->next;
  }
  
  int size = wb->size;
  release(&weight_store.lock);
  return size;
}
