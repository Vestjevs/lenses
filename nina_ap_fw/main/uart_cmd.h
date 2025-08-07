#pragma once

#include <stddef.h>
#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*uart_cmd_handler_t)(const char *line);

void uart_cmd_init(uart_port_t port_num,
                   int tx_pin,
                   int rx_pin,
                   int baud_rate,
                   uart_cmd_handler_t handler);

void uart_cmd_write(const char *data, size_t len);

void uart_cmd_write_line(const char *line);

#ifdef __cplusplus
}
#endif