#include "queue.h"
#include "util.h"
#include "interrupt_helper.h"

void k_init_queue(Queue* queue, void* queue_buffer, int max_data_count, int data_size) {
    queue->max_data_count = max_data_count;
    queue->data_size = data_size;
    queue->queue_array = queue_buffer;

    queue->put_index = 0;
    queue->get_index = 0;
    queue->last_operation_was_put = FALSE;
}

BOOL k_is_queue_full(const Queue* queue) {
    return (queue->get_index == queue->put_index && queue->last_operation_was_put); // it is indeed full.
}

BOOL k_is_queue_empty(const Queue* queue) {
    return (queue->get_index == queue->put_index && !(queue->last_operation_was_put));
}

BOOL k_put_queue(Queue* queue, const void* data) {
    if(k_is_queue_full(queue)) return FALSE;

    k_memcpy( (char*) queue->queue_array + (queue->data_size * queue->put_index), data, queue->data_size );

    queue->put_index = (queue->put_index + 1) % queue->max_data_count;
    queue->last_operation_was_put = TRUE;
    return TRUE;
}

BOOL k_get_queue(Queue* queue, void* data) {

    if (k_is_queue_empty(queue)) return FALSE;

    k_memcpy(data, (char*) queue->queue_array + (queue->data_size * queue->get_index), queue->data_size);

    queue->get_index = (queue->get_index + 1) % queue->max_data_count;
    queue->last_operation_was_put = FALSE;
    return TRUE;
}


