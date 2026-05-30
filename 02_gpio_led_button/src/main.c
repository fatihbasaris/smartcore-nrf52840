#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* DeviceTree alias */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define BUTTON_NODE DT_ALIAS(sw0)
#define PWR_NODE    DT_ALIAS(pwr0)

/* GPIO tanımı */
static const struct gpio_dt_spec led0 =
    GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static const struct gpio_dt_spec led1 =
    GPIO_DT_SPEC_GET(LED1_NODE, gpios);

static const struct gpio_dt_spec button =
    GPIO_DT_SPEC_GET(BUTTON_NODE, gpios);

static const struct gpio_dt_spec pwr =
    GPIO_DT_SPEC_GET(PWR_NODE, gpios);

int main(void)
{
        /* GPIO driver hazır mı kontrol et */
        if (!gpio_is_ready_dt(&led0) ||
            !gpio_is_ready_dt(&led1) ||
            !gpio_is_ready_dt(&button))
        {
                return 0;
        }
        /* LED pinini output yap */
        gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&led1, GPIO_OUTPUT_INACTIVE);
        gpio_pin_configure_dt(&button, GPIO_INPUT);

        gpio_pin_configure_dt(&pwr, GPIO_OUTPUT_INACTIVE);

        /* P0.26 -> 500 ms HIGH */
        gpio_pin_set_dt(&pwr, 1);

        k_msleep(500);

        gpio_pin_set_dt(&pwr, 0);

        while (1)
        {

                /* LED toggle */
                // gpio_pin_toggle_dt(&led0);
                // gpio_pin_toggle_dt(&led1);

                /* 500 ms bekle */
                // k_msleep(500);

                int val = gpio_pin_get_dt(&button);

                gpio_pin_set_dt(&led0, val);

                gpio_pin_set_dt(&led1, !val);

                k_msleep(50);
        }
}
