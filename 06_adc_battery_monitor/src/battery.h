#ifndef BATTERY_H
#define BATTERY_H

#include <stdint.h>

int battery_init(void);
int battery_read_mv(int32_t *battery_mv);
uint8_t battery_percent_from_mv(int32_t mv);

#endif