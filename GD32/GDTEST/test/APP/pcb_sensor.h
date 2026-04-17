#ifndef PCB_SENSOR_H
#define PCB_SENSOR_H

#include "gd32f10x.h"
#include <stdint.h>

void pcb_sensor_init(void);

uint8_t pcb_is_present(void);
uint8_t pcb_enter_event(void);
uint8_t pcb_leave_event(void);
uint8_t pcb_get_flag(void);
#endif

