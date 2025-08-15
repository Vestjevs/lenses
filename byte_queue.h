#ifndef BYTE_QUEUE_H
#define BYTE_QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_QUEUE_CAPACITY 32
#define BYTE_ARRAY_MAX_SIZE 1024

typedef struct {
    uint8_t data[BYTE_ARRAY_MAX_SIZE];
    uint16_t length;
} ByteArray;

typedef struct {
    ByteArray items[BYTE_QUEUE_CAPACITY];
    uint32_t head; // index of the current front element
    uint32_t tail; // index one past the last element
    uint32_t count; // number of elements currently in the queue
} ByteArrayQueue;

void byte_queue_init(ByteArrayQueue *queue);
bool byte_queue_is_empty(const ByteArrayQueue *queue);
bool byte_queue_is_full(const ByteArrayQueue *queue);
size_t byte_queue_size(const ByteArrayQueue *queue);

// Enqueues a copy of [data, length]. Returns false if full or length is invalid.
bool byte_queue_enqueue(ByteArrayQueue *queue, const uint8_t *data, uint16_t length);

// Dequeues the front element. If out is not NULL, copies it to *out. Returns false if empty.
bool byte_queue_dequeue(ByteArrayQueue *queue, ByteArray *out);

// Peeks at the front element without removing it. Returns false if empty.
bool byte_queue_peek(const ByteArrayQueue *queue, ByteArray *out);

// Returns a pointer to the front element stored inside the queue (NULL if empty).
const ByteArray *byte_queue_front_const(const ByteArrayQueue *queue);

// Returns a pointer to the back element stored inside the queue (NULL if empty).
const ByteArray *byte_queue_back_const(const ByteArrayQueue *queue);

#ifdef __cplusplus
}
#endif

#endif // BYTE_QUEUE_H