#ifndef BYTE_STACK_H
#define BYTE_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_STACK_CAPACITY 16
#define BYTE_STACK_ITEM_CAPACITY 1024

typedef struct {
    uint8_t bytes[BYTE_STACK_ITEM_CAPACITY];
    size_t length;
} ByteStackItem;

typedef struct {
    ByteStackItem items[BYTE_STACK_CAPACITY];
    size_t top;
} ByteStack;

void byte_stack_init(ByteStack* stack);
void byte_stack_clear(ByteStack* stack);
bool byte_stack_is_empty(const ByteStack* stack);
bool byte_stack_is_full(const ByteStack* stack);
size_t byte_stack_size(const ByteStack* stack);
bool byte_stack_push(ByteStack* stack, const uint8_t* data, size_t length);
bool byte_stack_pop(ByteStack* stack, ByteStackItem* out_item);
bool byte_stack_peek(const ByteStack* stack, ByteStackItem* out_item);

#ifdef __cplusplus
}
#endif

#endif /* BYTE_STACK_H */