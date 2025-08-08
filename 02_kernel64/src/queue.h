#pragma once
#include "types.h"
#pragma pack(push, 1)
typedef struct k_queue_manager_struct {
    int data_size;
    int max_data_count;

    void* queue_array;
    int put_index;
    int get_index;

    BOOL last_operation_was_put;
} Queue;

#pragma pack(pop)

/* Initialize the queue over a pre-allocated buffer */
void k_init_queue(Queue* queue,
                  void*   queue_buffer,
                  int     max_data_count,
                  int     data_size);

/* Return TRUE if the queue is full */
BOOL k_is_queue_full(const Queue* queue);

/* Return TRUE if the queue is empty */
BOOL k_is_queue_empty(const Queue* queue);

/* Enqueue one element; returns FALSE on overflow */
BOOL k_put_queue(Queue* queue,
                 const void* data);

/* Dequeue one element; returns FALSE on underflow */
BOOL k_get_queue(Queue* queue,
                 void*       data);