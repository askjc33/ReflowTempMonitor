#ifndef SPEED_SENSOR_H
#define SPEED_SENSOR_H

#include "gd32f10x.h"
#include <stdint.h>

void speed_sensor_init(void);
float speed_get_mm_s(void);
void speed_set_mm_per_pulse(float val);

/* 中断里已经自动更新时间，这里提供状态接口 */
uint8_t speed_is_valid(void);
uint32_t speed_get_period_us(void);

#endif

