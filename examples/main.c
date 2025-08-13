#include <stdio.h>
#include "byte_stack.h"

int main(void) {
    ByteStack stack;
    byte_stack_init(&stack);

    for (int i = 0; i < 20; ++i) {
        uint8_t value = (uint8_t)i;
        if (!byte_stack_push(&stack, value)) {
            printf("Push %d failed: stack is full at size %zu\n", i, byte_stack_size(&stack));
            break;
        }
    }

    uint8_t top_value;
    if (byte_stack_peek(&stack, &top_value)) {
        printf("Top element: %u\n", (unsigned)top_value);
    }

    while (byte_stack_pop(&stack, &top_value)) {
        printf("Popped: %u, size now: %zu\n", (unsigned)top_value, byte_stack_size(&stack));
    }

    if (!byte_stack_pop(&stack, &top_value)) {
        printf("Pop failed: stack is empty\n");
    }

    return 0;
}