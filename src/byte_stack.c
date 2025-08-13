#include "byte_stack.h"

void byte_stack_init(ByteStack* stack) {
    if (!stack) return;
    stack->top = 0u;
}

void byte_stack_clear(ByteStack* stack) {
    if (!stack) return;
    stack->top = 0u;
}

bool byte_stack_is_empty(const ByteStack* stack) {
    return !stack || stack->top == 0u;
}

bool byte_stack_is_full(const ByteStack* stack) {
    return stack && stack->top >= BYTE_STACK_CAPACITY;
}

size_t byte_stack_size(const ByteStack* stack) {
    return stack ? stack->top : 0u;
}

bool byte_stack_push(ByteStack* stack, uint8_t value) {
    if (!stack || byte_stack_is_full(stack)) {
        return false;
    }
    stack->data[stack->top++] = value;
    return true;
}

bool byte_stack_pop(ByteStack* stack, uint8_t* out_value) {
    if (!stack || byte_stack_is_empty(stack)) {
        return false;
    }
    uint8_t value = stack->data[stack->top - 1u];
    stack->top--;
    if (out_value) {
        *out_value = value;
    }
    return true;
}

bool byte_stack_peek(const ByteStack* stack, uint8_t* out_value) {
    if (!stack || byte_stack_is_empty(stack) || !out_value) {
        return false;
    }
    *out_value = stack->data[stack->top - 1u];
    return true;
}