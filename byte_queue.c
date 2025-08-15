#include "byte_queue.h"
#include <string.h>

void byte_queue_init(ByteArrayQueue *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
}

bool byte_queue_is_empty(const ByteArrayQueue *queue) {
    return queue->count == 0;
}

bool byte_queue_is_full(const ByteArrayQueue *queue) {
    return queue->count == BYTE_QUEUE_CAPACITY;
}

size_t byte_queue_size(const ByteArrayQueue *queue) {
    return (size_t)queue->count;
}

bool byte_queue_enqueue(ByteArrayQueue *queue, const uint8_t *data, uint16_t length) {
    if (byte_queue_is_full(queue)) {
        return false;
    }
    if (length > BYTE_ARRAY_MAX_SIZE) {
        return false;
    }

    ByteArray *slot = &queue->items[queue->tail];
    if (length > 0 && data != NULL) {
        memcpy(slot->data, data, (size_t)length);
    }
    slot->length = length;

    queue->tail = (queue->tail + 1u) % BYTE_QUEUE_CAPACITY;
    queue->count++;
    return true;
}

bool byte_queue_dequeue(ByteArrayQueue *queue, ByteArray *out) {
    if (byte_queue_is_empty(queue)) {
        return false;
    }

    ByteArray *front = &queue->items[queue->head];
    if (out != NULL) {
        if (front->length > 0) {
            memcpy(out->data, front->data, (size_t)front->length);
        }
        out->length = front->length;
    }

    queue->head = (queue->head + 1u) % BYTE_QUEUE_CAPACITY;
    queue->count--;
    return true;
}

bool byte_queue_peek(const ByteArrayQueue *queue, ByteArray *out) {
    if (byte_queue_is_empty(queue)) {
        return false;
    }

    const ByteArray *front = &queue->items[queue->head];
    if (out != NULL) {
        if (front->length > 0) {
            memcpy(out->data, front->data, (size_t)front->length);
        }
        out->length = front->length;
    }
    return true;
}

const ByteArray *byte_queue_front_const(const ByteArrayQueue *queue) {
    if (byte_queue_is_empty(queue)) {
        return NULL;
    }
    return &queue->items[queue->head];
}

const ByteArray *byte_queue_back_const(const ByteArrayQueue *queue) {
    if (byte_queue_is_empty(queue)) {
        return NULL;
    }
    uint32_t idx = (queue->tail == 0u) ? (BYTE_QUEUE_CAPACITY - 1u) : (queue->tail - 1u);
    return &queue->items[idx];
}