#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "gpio_app.h"
#include "ble_service.h"

int main(void)
{
    int ret;

    printk("APP START\r\n");

    ret = gpio_app_init();
    if (ret < 0) {

        printk("GPIO init failed\r\n");

        return 0;
    }

    ret = ble_service_init();
    if (ret < 0) {

        printk("BLE init failed\r\n");

        return 0;
    }

    int last_button_state = 0;

    while (1)
    {
        int button_state = gpio_app_button_get();

        if (button_state != last_button_state)
        {
            last_button_state = button_state;

            if (button_state)
            {
                gpio_app_led_on();

                printk("BUTTON PRESSED\r\n");

                ble_service_notify("LED_ON");
            }
            else
            {
                gpio_app_led_off();

                printk("BUTTON RELEASED\r\n");

                ble_service_notify("LED_OFF");
            }
        }

        k_sleep(K_MSEC(50));
    }
}