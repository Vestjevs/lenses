#include <stdio.h>
#include <string.h>
#include "byte_stack.h"

static void print_item(const ByteStackItem* item) {
    printf("Item length=%zu, bytes:", item->length);
    for (size_t i = 0; i < item->length; ++i) {
        printf(" %u", (unsigned)item->bytes[i]);
    }
    printf("\n");
}

int main(void) {
    ByteStack stack;
    byte_stack_init(&stack);

    /* Push a few varying-length arrays */
    uint8_t a[] = {1, 2, 3};
    uint8_t b[BYTE_STACK_ITEM_CAPACITY];
    for (size_t i = 0; i < sizeof(b); ++i) b[i] = (uint8_t)(i & 0xFF);
    uint8_t c[] = {42};

    if (!byte_stack_push(&stack, a, sizeof(a))) {
        printf("Push a failed\n");
    }
    if (!byte_stack_push(&stack, b, 1024)) {
        printf("Push b failed\n");
    }
    if (!byte_stack_push(&stack, c, sizeof(c))) {
        printf("Push c failed\n");
    }

    /* Try to push an oversized array (should fail) */
    uint8_t too_big[BYTE_STACK_ITEM_CAPACITY + 1];
    memset(too_big, 0, sizeof(too_big));
    if (!byte_stack_push(&stack, too_big, sizeof(too_big))) {
        printf("Push too_big failed as expected (len=%zu)\n", sizeof(too_big));
    }

    /* Peek */
    ByteStackItem item;
    if (byte_stack_peek(&stack, &item)) {
        printf("Top element after pushes: ");
        print_item(&item);
    }

    /* Pop all */
    while (byte_stack_pop(&stack, &item)) {
        printf("Popped: ");
        print_item(&item);
        printf("Size now: %zu\n", byte_stack_size(&stack));
    }

    if (!byte_stack_pop(&stack, &item)) {
        printf("Pop failed: stack is empty\n");
    }

    return 0;
}