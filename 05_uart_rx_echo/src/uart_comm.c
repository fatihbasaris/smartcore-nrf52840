#include "uart_comm.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

int uart_comm_init(void)
{
    if (!device_is_ready(uart_dev)) {
        return -1;
    }

    return 0;
}

void uart_comm_send_char(char c)
{
    uart_poll_out(uart_dev, c);
}

void uart_comm_send_str(const char *str)
{
    while (*str) {
        uart_poll_out(uart_dev, *str++);
    }
}

int uart_comm_read_char(uint8_t *ch)
{
    return uart_poll_in(uart_dev, ch);
}

int uart_comm_read_line(char *buf, size_t buf_size, int32_t timeout_ms)
{
    uint8_t ch;
    size_t pos = 0;
    int64_t start = k_uptime_get();

    if (buf == NULL || buf_size == 0) {
        return -1;
    }

    while (1) {
        if (uart_poll_in(uart_dev, &ch) == 0) {

            if (ch == '\r' || ch == '\n') {
                if (pos > 0) {
                    buf[pos] = '\0';
                    return (int)pos;
                }
            } else {
                if (pos < buf_size - 1) {
                    buf[pos++] = (char)ch;
                } else {
                    buf[pos] = '\0';
                    return -2;
                }
            }
        }

        if (timeout_ms >= 0) {
            if ((k_uptime_get() - start) >= timeout_ms) {
                buf[pos] = '\0';
                return 0;
            }
        }

        k_msleep(2);
    }
}