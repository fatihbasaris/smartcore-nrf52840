#ifndef GPIO_APP_H
#define GPIO_APP_H

#include <zephyr/drivers/gpio.h>

int gpio_app_init(void);

void gpio_app_led_on(void);
void gpio_app_led_off(void);

int gpio_app_button_get(void);

#endif