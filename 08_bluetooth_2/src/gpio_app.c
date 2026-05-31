#include "gpio_app.h"
#include <zephyr/sys/printk.h>

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);

int gpio_app_init(void)
{
    int ret;

    if (!device_is_ready(led.port)) {
        printk("LED device not ready\r\n");
        return -1;
    }

    if (!device_is_ready(button.port)) {
        printk("Button device not ready\r\n");
        return -1;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("LED config failed: %d\r\n", ret);
        return ret;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        printk("Button config failed: %d\r\n", ret);
        return ret;
    }

    return 0;
}

void gpio_app_led_on(void)
{
    gpio_pin_set_dt(&led, 1);
}

void gpio_app_led_off(void)
{
    gpio_pin_set_dt(&led, 0);
}

int gpio_app_button_get(void)
{
    return gpio_pin_get_dt(&button);
}