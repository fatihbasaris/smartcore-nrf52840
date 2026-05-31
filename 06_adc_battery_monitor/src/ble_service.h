#ifndef BLE_SERVICE_H
#define BLE_SERVICE_H

#include <stdint.h>
#include <stddef.h>

int ble_service_init(void);
int ble_service_notify_battery(uint8_t percent, int32_t battery_mv);

#endif