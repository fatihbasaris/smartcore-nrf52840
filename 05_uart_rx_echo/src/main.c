#include <zephyr/kernel.h>
#include "uart_comm.h"

/*
counter her saniye artar.
Her saniye UART'a yazdırılır.
Eğer kullanıcı veri gönderirse, gelen satır işlenir.
Veri gelmesini beklerken program bloke olmamalı.

Beklenen çıktı:

Counter: 1
Counter: 2
Counter: 3
Counter: 4

Kullanıcı yazarsa:

hello
Aldim: hello

sonra tekrar:

Counter: 5
Counter: 6
Counter: 7
...
*/

#define RX_LINE_SIZE 64

int main(void)
{
    char line[RX_LINE_SIZE];
    uint32_t counter = 0;

    if (uart_comm_init() != 0) {
        return 0;
    }

    uart_comm_send_str("Basladi\r\n");

    while (1)
    {
        int len = uart_comm_read_line(line,
                                      sizeof(line),
                                      1000);   // 1 saniye bekle

        if (len > 0)
        {
            uart_comm_send_str("Aldim: ");
            uart_comm_send_str(line);
            uart_comm_send_str("\r\n");
        }
        else
        {
            counter++;

            char msg[32];
            snprintf(msg,
                     sizeof(msg),
                     "Counter: %lu\r\n",
                     (unsigned long)counter);

            uart_comm_send_str(msg);
        }
    }
}


/*

karakter karakter değil satır tamamlanınca 

hello
Aldim: hello

fth
Aldim: fth gibi

#include <zephyr/kernel.h>
#include "uart_comm.h"

#define RX_LINE_SIZE 64

int main(void)
{
    char line[RX_LINE_SIZE];

    if (uart_comm_init() != 0) {
        return 0;
    }

    uart_comm_send_str("\r\nUART moduler test basladi\r\n");
    uart_comm_send_str("Bir metin yazip Enter'a basin.\r\n");

    while (1) {
        int len = uart_comm_read_line(line, sizeof(line), -1);

        if (len > 0) {
            uart_comm_send_str("Aldim: ");
            uart_comm_send_str(line);
            uart_comm_send_str("\r\n");
        } else if (len == -2) {
            uart_comm_send_str("Hata: satir buffer doldu\r\n");
        }
    }
}*/

/*
tek tek alma testi

hello
[h]
[e]
[l]
[l]
[o]
[
]


#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>

#define UART_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);

int main(void)
{
    uint8_t ch;

    if (!device_is_ready(uart_dev)) {
        return 0;
    }

    while (1) {

        if (uart_poll_in(uart_dev, &ch) == 0) {

            uart_poll_out(uart_dev, '[');
            uart_poll_out(uart_dev, ch);
            uart_poll_out(uart_dev, ']');
            uart_poll_out(uart_dev, '\r');
            uart_poll_out(uart_dev, '\n');
        }

        k_msleep(10);
    }
}*/
