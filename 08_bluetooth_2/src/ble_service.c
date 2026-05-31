#include "ble_service.h"
#include "gpio_app.h"

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/printk.h>

#include <string.h>

static bool notify_enabled = false;
static char notify_value[32] = "SYSTEM_READY";

#define BT_UUID_FTH_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

#define BT_UUID_FTH_LED_VAL \
    BT_UUID_128_ENCODE(0x12345679, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

static struct bt_uuid_128 fth_service_uuid =
    BT_UUID_INIT_128(BT_UUID_FTH_SERVICE_VAL);

static struct bt_uuid_128 fth_led_uuid =
    BT_UUID_INIT_128(BT_UUID_FTH_LED_VAL);

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

static void send_notify(void);

static ssize_t read_value(struct bt_conn *conn,
                          const struct bt_gatt_attr *attr,
                          void *buf,
                          uint16_t len,
                          uint16_t offset)
{
    return bt_gatt_attr_read(conn,
                             attr,
                             buf,
                             len,
                             offset,
                             notify_value,
                             strlen(notify_value));
}

static void ccc_cfg_changed(const struct bt_gatt_attr *attr,
                            uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);

    printk("Notify %s\r\n",
           notify_enabled ? "enabled" : "disabled");
}

static ssize_t write_led(struct bt_conn *conn,
                         const struct bt_gatt_attr *attr,
                         const void *buf,
                         uint16_t len,
                         uint16_t offset,
                         uint8_t flags)
{
    const uint8_t *data = buf;

    if (len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (data[0] == '1') {

        gpio_app_led_on();

        strcpy(notify_value, "LED_ON");

        printk("BLE WRITE: 1 -> LED ON\r\n");

        send_notify();

    } else if (data[0] == '0') {

        gpio_app_led_off();

        strcpy(notify_value, "LED_OFF");

        printk("BLE WRITE: 0 -> LED OFF\r\n");

        send_notify();
    }

    return len;
}

BT_GATT_SERVICE_DEFINE(fth_svc,

    BT_GATT_PRIMARY_SERVICE(&fth_service_uuid),

    BT_GATT_CHARACTERISTIC(&fth_led_uuid.uuid,
                           BT_GATT_CHRC_READ |
                           BT_GATT_CHRC_WRITE |
                           BT_GATT_CHRC_NOTIFY,

                           BT_GATT_PERM_READ |
                           BT_GATT_PERM_WRITE,

                           read_value,
                           write_led,
                           notify_value),

    BT_GATT_CCC(ccc_cfg_changed,
                BT_GATT_PERM_READ |
                BT_GATT_PERM_WRITE)
);

static void send_notify(void)
{
    if (!notify_enabled) {
        return;
    }

    bt_gatt_notify(NULL,
                   &fth_svc.attrs[2],
                   notify_value,
                   strlen(notify_value));
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed: %u\r\n", err);
        return;
    }

    gpio_app_led_on();

    printk("Connected\r\n");
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    gpio_app_led_off();

    printk("Disconnected: %u\r\n", reason);

    bt_le_adv_start(&adv_param,
                    ad,
                    ARRAY_SIZE(ad),
                    NULL,
                    0);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

int ble_service_init(void)
{
    int ret;

    ret = bt_enable(NULL);
    if (ret) {

        printk("Bluetooth init failed: %d\r\n", ret);

        return ret;
    }

    printk("Bluetooth initialized\r\n");

    ret = bt_le_adv_start(&adv_param,
                          ad,
                          ARRAY_SIZE(ad),
                          NULL,
                          0);

    if (ret) {

        printk("Advertising failed: %d\r\n", ret);

        return ret;
    }

    printk("Advertising started\r\n");

    return 0;
}

void ble_service_notify(const char *msg)
{
    strcpy(notify_value, msg);

    send_notify();
}