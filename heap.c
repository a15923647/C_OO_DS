#include <stdio.h>
#include <stdlib.h>
#define PR(x) ((int) (x)/2)
#define LC(x) ((int)(x)*2)
#define RC(x) ((int)(x)*2+1)
#define int_swap(a, b) do { \
  int *_a = a, *_b = b; \
  int tmp = *_a; \
  *_a = *_b; \
  *_b = tmp; \
} while(0)
#define min_ptr(a, b) (*(a) < *(b)) ? (a) : (b)

typedef struct heap {
  size_t buffer_size, length;
  int *ptr;
  //input -> output
  int (*insert) (struct heap *, int);
  int (*extract) (struct heap *); //void -> top element
  int (*swim) (struct heap *, int); //child id -> parent id
  int (*sink) (struct heap *, int); //parent id -> child id
  void (*dest) (struct heap **);//destructor
} Heap;

enum FAIL {
  NO_ENOUGH_ELEMENT,
  NO_SPACE_FOR_NEW_ELEMENT
};

static int insert_impl(Heap *self, int data) {
  if (self->length >= self->buffer_size) return NO_SPACE_FOR_NEW_ELEMENT;
  self->ptr[++self->length] = data;
  int insert_res = self->swim(self, self->length);
  return insert_res;
}

static int swim_impl(Heap *self, int child_id) {
  if (child_id == 1) return 1;
  int parent = PR(child_id);
  if (self->ptr[parent] > self->ptr[child_id]) //min heap
    int_swap(self->ptr+parent, self->ptr+child_id);
  return self->swim(self, parent);
}

static int extract_impl(Heap *self) {
  if (self->length == 0) return NO_ENOUGH_ELEMENT;
  int ret_val = self->ptr[1];
  self->ptr[1] = self->ptr[self->length];
  self->length--; //remove an element
  self->sink(self, 1);
  return ret_val;
}

static int sink_impl(Heap *self, int parent_id) {
  //if parent_id is in the button layer, return
  int lc = LC(parent_id), rc = RC(parent_id);
  //left child is not exists
  if (lc > self->length) 
    return parent_id;
  //left child exists, if right child not exists, dive in left child
  if (rc > self->length) {
    //only lc, no rc
    if (self->ptr[parent_id] > self->ptr[lc]) 
      int_swap(self->ptr+parent_id, self->ptr+lc);
    return self->sink(self, lc); // or simply return lc (since heap is a complete tree)
  }
  //both children exist.
  int *mptr = min_ptr(self->ptr+lc, self->ptr+rc);
  if (self->ptr[parent_id] > *mptr) int_swap(self->ptr+parent_id, mptr);
  return self->sink(self, mptr-self->ptr);
}

void show_heap(Heap *self) {
  int offset, idx=1;
  puts("show_heap is called");
  for (offset = 0; idx <= self->length; offset++) {
    int next_start = (2 << offset);
    for (idx = (1 << offset); idx <= self->length && idx < next_start; idx++) {
      printf("heap[%d]=%d ", idx, self->ptr[idx]);
    }
    printf("\n");
  }
}

static void dest_impl(Heap **self) {
  free((*self)->ptr);
  free((*self));
  *self = NULL;
}

int init_heap(Heap **o, size_t buf_size) {
  if (NULL == (*o = malloc(sizeof(Heap)))) return -1;
  (*o)->buffer_size = buf_size+1;
  (*o)->ptr = malloc((buf_size+1) * sizeof(int));
  (*o)->length = 0;
  (*o)->insert = insert_impl;
  (*o)->extract = extract_impl;
  (*o)->swim = swim_impl;
  (*o)->sink = sink_impl;
  (*o)->dest = dest_impl;
  return 0;
}

int main() {
  int insert_res = 0;
  Heap *h;
  init_heap(&h, 64);
  show_heap(h);
  puts("insert 3");
  insert_res = h->insert(h, 3);
  puts("insert 10");
  insert_res = h->insert(h, 10);
  puts("insert 4");
  insert_res = h->insert(h, 4);
  show_heap(h);
  printf("extract %d\n", h->extract(h));
  show_heap(h);
  puts("insert 4");
  insert_res = h->insert(h, 4);
  puts("insert 6");
  insert_res = h->insert(h, 6);
  puts("insert 8");
  insert_res = h->insert(h, 8);
  show_heap(h);
  h->dest(&h);
  return 0;
}
