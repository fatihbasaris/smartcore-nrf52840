#include "ble_service.h"

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/printk.h>

static bool notify_enabled;

static void ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    printk("Battery notify %s\r\n", notify_enabled ? "enabled" : "disabled");
}

BT_GATT_SERVICE_DEFINE(battery_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_128(
        0x10, 0x00, 0x00, 0x00, 0x5f, 0x9b, 0x4f, 0x7a,
        0x9b, 0x3a, 0x7e, 0x01, 0x00, 0x00, 0xab, 0xcd)),

    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_128(
        0x10, 0x01, 0x00, 0x00, 0x5f, 0x9b, 0x4f, 0x7a,
        0x9b, 0x3a, 0x7e, 0x01, 0x00, 0x00, 0xab, 0xcd),
        BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_NONE,
        NULL, NULL, NULL),

    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

int ble_service_init(void)
{
    notify_enabled = false;
    return 0;
}

int ble_service_notify_battery(uint8_t percent, int32_t battery_mv)
{
    uint8_t data[4];
    uint8_t alarm_flags = 0;

    if (!notify_enabled) {
        return -EACCES;
    }

    if (battery_mv <= 3600) {
        alarm_flags |= BIT(0);   // low battery
    }

    if (battery_mv <= 3400) {
        alarm_flags |= BIT(1);   // critical battery
    }

    data[0] = percent;
    data[1] = (uint8_t)(battery_mv & 0xFF);
    data[2] = (uint8_t)((battery_mv >> 8) & 0xFF);
    data[3] = alarm_flags;

    return bt_gatt_notify(NULL, &battery_svc.attrs[1], data, sizeof(data));
}