#include <stdio.h>
#include <string.h>
#include "byte_queue.h"

static void print_item(const char *label, const ByteArray *item) {
    printf("%s len=%u : ", label, (unsigned)item->length);
    for (unsigned i = 0; i < item->length; ++i) {
        printf("%02X ", item->data[i]);
    }
    printf("\n");
}

int main(void) {
    ByteArrayQueue q;
    byte_queue_init(&q);

    uint8_t a[] = {1,2,3};
    uint8_t b[] = {0xAA, 0xBB, 0xCC, 0xDD};

    printf("Enqueue a (3) -> %s\n", byte_queue_enqueue(&q, a, (uint16_t)sizeof(a)) ? "ok" : "fail");
    printf("Enqueue b (4) -> %s\n", byte_queue_enqueue(&q, b, (uint16_t)sizeof(b)) ? "ok" : "fail");

    printf("Queue size: %zu\n", byte_queue_size(&q));

    ByteArray front;
    if (byte_queue_peek(&q, &front)) {
        print_item("Peek", &front);
    }

    ByteArray out;
    while (byte_queue_dequeue(&q, &out)) {
        print_item("Dequeue", &out);
    }

    printf("Queue empty: %s\n", byte_queue_is_empty(&q) ? "yes" : "no");

    // Test max-length boundary
    ByteArray big;
    memset(big.data, 0x5A, BYTE_ARRAY_MAX_SIZE);
    big.length = BYTE_ARRAY_MAX_SIZE;
    printf("Enqueue big (1024) -> %s\n", byte_queue_enqueue(&q, big.data, big.length) ? "ok" : "fail");
    ByteArray peeked;
    if (byte_queue_peek(&q, &peeked)) {
        printf("Peek len=%u first=%02X last=%02X\n", (unsigned)peeked.length, peeked.data[0], peeked.data[peeked.length-1]);
    }

    return 0;
}