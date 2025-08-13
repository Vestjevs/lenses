#ifndef BYTE_STACK_H
#define BYTE_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BYTE_STACK_CAPACITY 16

typedef struct {
    uint8_t data[BYTE_STACK_CAPACITY];
    size_t top; /* number of elements currently stored */
} ByteStack;

void byte_stack_init(ByteStack* stack);
void byte_stack_clear(ByteStack* stack);
bool byte_stack_is_empty(const ByteStack* stack);
bool byte_stack_is_full(const ByteStack* stack);
size_t byte_stack_size(const ByteStack* stack);
bool byte_stack_push(ByteStack* stack, uint8_t value);
bool byte_stack_pop(ByteStack* stack, uint8_t* out_value);
bool byte_stack_peek(const ByteStack* stack, uint8_t* out_value);

#ifdef __cplusplus
}
#endif

#endif /* BYTE_STACK_H */