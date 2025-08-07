#include "uart_cmd.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"

#define UART_CMD_TASK_STACK 4096
#define UART_CMD_TASK_PRIO  10
#define UART_RX_BUF_SIZE    2048
#define UART_TX_BUF_SIZE    2048

static const char *TAG = "uart_cmd";

static uart_port_t g_uart_port = UART_NUM_1;
static uart_cmd_handler_t g_handler = NULL;

static void uart_cmd_task(void *arg) {
    uint8_t *rx_buffer = (uint8_t *) malloc(UART_RX_BUF_SIZE);
    if (!rx_buffer) {
        ESP_LOGE(TAG, "Failed to allocate RX buffer");
        vTaskDelete(NULL);
        return;
    }

    // Line assembly buffer
    char line_buf[512];
    size_t line_len = 0;

    while (1) {
        int len = uart_read_bytes(g_uart_port, rx_buffer, UART_RX_BUF_SIZE, pdMS_TO_TICKS(100));
        if (len > 0) {
            for (int i = 0; i < len; i++) {
                char c = (char)rx_buffer[i];
                if (c == '\r') {
                    // ignore
                    continue;
                }
                if (c == '\n') {
                    // terminate line
                    line_buf[line_len] = '\0';
                    if (line_len > 0 && g_handler) {
                        g_handler(line_buf);
                    }
                    line_len = 0;
                } else {
                    if (line_len + 1 < sizeof(line_buf)) {
                        line_buf[line_len++] = c;
                    } else {
                        // overflow -> reset line
                        ESP_LOGW(TAG, "Line too long, flushing");
                        line_len = 0;
                    }
                }
            }
        }
    }
}

void uart_cmd_init(uart_port_t port_num,
                   int tx_pin,
                   int rx_pin,
                   int baud_rate,
                   uart_cmd_handler_t handler) {
    g_uart_port = port_num;
    g_handler = handler;

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    ESP_ERROR_CHECK(uart_driver_install(g_uart_port, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(g_uart_port, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(g_uart_port, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(uart_cmd_task, "uart_cmd_task", UART_CMD_TASK_STACK, NULL, UART_CMD_TASK_PRIO, NULL);
}

void uart_cmd_write(const char *data, size_t len) {
    if (!data || len == 0) return;
    uart_write_bytes(g_uart_port, data, len);
}

void uart_cmd_write_line(const char *line) {
    if (!line) return;
    uart_write_bytes(g_uart_port, line, strlen(line));
    const char crlf[] = "\r\n";
    uart_write_bytes(g_uart_port, crlf, sizeof(crlf) - 1);
}