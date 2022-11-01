#ifndef CB_QUEUE_C
#define CB_QUEUE_C

#include <stdlib.h>  // size_t

typedef struct q queue_t;
queue_t* queue_initialize(size_t capacity, size_t data_size);
void queue_destroy(queue_t* queue);
int push(queue_t* queue, const void* item);
int pop(queue_t* queue, void* item);

#endif
