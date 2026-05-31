#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led =
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static bool restart_adv = false;

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),

    BT_DATA(BT_DATA_NAME_COMPLETE,
            CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static const struct bt_le_adv_param adv_param = {
    .id = BT_ID_DEFAULT,
    .sid = 0,
    .secondary_max_skip = 0,
    .options = BT_LE_ADV_OPT_CONN,
    .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
    .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    .peer = NULL,
};

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed, err: %u\r\n", err);
        return;
    }

    printk("Connected\r\n");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Disconnected, reason: %u\r\n", reason);
    restart_adv = true;
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

int main(void)
{
    int err;
    bool led_state = false;

    printk("BLE Connect Callback + LED Blink Test\r\n");

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

    err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        printk("Advertising failed: %d\r\n", err);
        return 0;
    }

    printk("Advertising started: %s\r\n", CONFIG_BT_DEVICE_NAME);

    while (1) {
        if (restart_adv) {
            restart_adv = false;

            err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
            if (err) {
                printk("Advertising restart failed: %d\r\n", err);
            } else {
                printk("Advertising restarted\r\n");
            }
        }

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