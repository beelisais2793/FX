#include <stdio.h>   // getchar
#include <string.h>  // memset
#include "queue.h"

typedef struct f {
  int n[1000000];  // 4MB
} foo;

int main() {
  foo* f = malloc(sizeof(foo));
  memset(f, 0, sizeof(foo));

  queue_t* q = queue_initialize(16, sizeof(foo));

  printf("pushed ");
  for (f->n[0] = 0; push(q, f); f->n[0]++) printf("%d ", f->n[0]);
  printf("\n");

  printf("popped ");
  while (pop(q, f)) printf("%d ", f->n[0]);
  printf("\n");

  queue_destroy(q);

  free(f);
}
