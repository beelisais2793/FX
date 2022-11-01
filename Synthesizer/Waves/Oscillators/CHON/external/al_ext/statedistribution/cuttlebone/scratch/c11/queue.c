#include "queue.h"
#include "stdatomic.h"
#include <stdlib.h>  // size_t
#include <string.h>  // memcpy
#include <stdio.h>   // printf

typedef struct q {
  size_t capacity, data_size;
  atomic_size_t head, tail;
  void* data;
} queue_t;

size_t nlpo2(size_t x) {
  x--;
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  return (x + 1);
}

queue_t* queue_initialize(size_t capacity, size_t data_size) {
  queue_t* queue = malloc(sizeof(queue_t));
  memset(queue, 0, sizeof(queue_t));
  queue->capacity = nlpo2(capacity);
  queue->data_size = data_size;
  queue->data = malloc(capacity * data_size);
  return queue;
}

void queue_destroy(queue_t* queue) {
  free(queue->data);
  free(queue);
}

int push(queue_t* queue, const void* item) {
  const size_t current_tail =
      atomic_load_explicit(&queue->tail, memory_order_relaxed);
  const size_t next_tail = (current_tail + 1) & (queue->capacity - 1);
  if (next_tail != atomic_load_explicit(&queue->head, memory_order_acquire)) {
    memcpy(queue->data + (current_tail * queue->data_size), item,
           queue->data_size);
    atomic_store_explicit(&queue->tail, next_tail, memory_order_release);
    return 1;
  }
  return 0;  // full queue
}

int pop(queue_t* queue, void* item) {
  const size_t current_head =
      atomic_load_explicit(&queue->head, memory_order_relaxed);
  if (current_head == atomic_load_explicit(&queue->tail, memory_order_acquire))
    return 0;  // empty queue
  memcpy(item, queue->data + (current_head * queue->data_size),
         queue->data_size);
  atomic_store_explicit(&queue->head,
                        (current_head + 1) & (queue->capacity - 1),
                        memory_order_release);
  return 1;
}

