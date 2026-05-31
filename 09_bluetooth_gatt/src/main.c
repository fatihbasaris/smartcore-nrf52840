#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),

    BT_DATA(BT_DATA_NAME_COMPLETE,
            CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

int main(void)
{
    int err;
    bool led_state = false;

    printk("Step 1: LED blink + USB printk\r\n");

    if (!device_is_ready(led.port)) {
        printk("LED device not ready\r\n");
        return 0;
    }

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (err < 0) {
        printk("LED config failed: %d\r\n", err);
        return 0;
    }

    err = bt_enable(NULL);
if (err) {
    printk("Bluetooth init failed: %d\r\n", err);
    return 0;
}

printk("Bluetooth initialized\r\n");

err = bt_le_adv_start(BT_LE_ADV_CONN_NAME,
                      ad,
                      ARRAY_SIZE(ad),
                      NULL,
                      0);

if (err) {
    printk("Advertising failed: %d\r\n", err);
    return 0;
}

printk("Advertising started: %s\r\n",
       CONFIG_BT_DEVICE_NAME);

    while (1) {
        led_state = !led_state;
        gpio_pin_set_dt(&led, led_state);

        if (led_state) {
            printk("LED ON\r\n");
        } else {
            printk("LED OFF\r\n");
        }

        k_sleep(K_SECONDS(1));
    }
}
