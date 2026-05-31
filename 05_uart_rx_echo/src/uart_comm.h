#ifndef UART_COMM_H
#define UART_COMM_H

#include <stdint.h>
#include <stddef.h>

int uart_comm_init(void);
void uart_comm_send_char(char c);
void uart_comm_send_str(const char *str);
int uart_comm_read_char(uint8_t *ch);
int uart_comm_read_line(char *buf, size_t buf_size, int32_t timeout_ms);

#endif