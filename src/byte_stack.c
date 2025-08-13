#include "byte_stack.h"
#include <string.h>

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

bool byte_stack_push(ByteStack* stack, const uint8_t* data, size_t length) {
    if (!stack || !data) {
        return false;
    }
    if (byte_stack_is_full(stack) || length > BYTE_STACK_ITEM_CAPACITY) {
        return false;
    }
    ByteStackItem* item = &stack->items[stack->top];
    if (length > 0) {
        memcpy(item->bytes, data, length);
    }
    item->length = length;
    stack->top++;
    return true;
}

bool byte_stack_pop(ByteStack* stack, ByteStackItem* out_item) {
    if (!stack || byte_stack_is_empty(stack)) {
        return false;
    }
    ByteStackItem* item = &stack->items[stack->top - 1u];
    if (out_item) {
        if (item->length > 0) {
            memcpy(out_item->bytes, item->bytes, item->length);
        }
        out_item->length = item->length;
    }
    stack->top--;
    return true;
}

bool byte_stack_peek(const ByteStack* stack, ByteStackItem* out_item) {
    if (!stack || byte_stack_is_empty(stack) || !out_item) {
        return false;
    }
    const ByteStackItem* item = &stack->items[stack->top - 1u];
    if (item->length > 0) {
        memcpy(out_item->bytes, item->bytes, item->length);
    }
    out_item->length = item->length;
    return true;
}