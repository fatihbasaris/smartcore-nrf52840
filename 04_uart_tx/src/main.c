#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

int main(void)
{
    const char msg[] = "Merhaba dunya\r\n";

    if (!device_is_ready(uart_dev)) {
        return 0;
    }

    while (1) {
        for (int i = 0; msg[i] != '\0'; i++) {
            uart_poll_out(uart_dev, msg[i]);
        }

        k_msleep(1000);
    }
}
