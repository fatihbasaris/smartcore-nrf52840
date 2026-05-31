#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/conn.h>

#include "ble_service.h"

#include "battery.h"

#define PERIODIC_WAKEUP_SECONDS 60

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

int main(void)
{
    int err;
    int32_t battery_mv;
    uint8_t battery_percent;

    usb_enable(NULL);
    k_sleep(K_MSEC(1000));

    printk("\nFTH E73 Battery Monitor started\n");

    if (device_is_ready(led.port))
    {
        gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    }

    err = battery_init();
    if (err)
    {
        printk("Battery init failed: %d\n", err);
        return 0;
    }

    printk("Battery ADC initialized\n");

    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed: %d\r\n", err);
        return 0;
    }

    printk("Bluetooth initialized\r\n");

    ble_service_init();

    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err)
    {
        printk("Advertising failed: %d\r\n", err);
        return 0;
    }

    printk("Advertising as %s\r\n", CONFIG_BT_DEVICE_NAME);

    while (1)
{
    err = battery_read_mv(&battery_mv);

    if (err == 0)
    {
        battery_percent = battery_percent_from_mv(battery_mv);

        printk("Battery: %ld mV, %u %%\r\n",
               (long)battery_mv,
               battery_percent);

        err = ble_service_notify_battery(battery_percent, battery_mv);
        if (err == 0)
        {
            printk("BLE battery notify sent\r\n");
        }

        if (device_is_ready(led.port))
        {
            gpio_pin_toggle_dt(&led);
        }
    }
    else
    {
        printk("Battery read failed: %d\r\n", err);
    }

    k_sleep(K_SECONDS(PERIODIC_WAKEUP_SECONDS));
}
}
