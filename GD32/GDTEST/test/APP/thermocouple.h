#ifndef THERMOCOUPLE_H
#define THERMOCOUPLE_H

#include <stdint.h>

double ads_code_to_uv(int32_t code, double vref_uv, uint8_t gain);
double k_cjc_temp_to_uv(double temp_c);
double k_uv_to_temp_c(double uv);
double k_tc_compensated_temp_c(int32_t code, double cjc_temp_c, double vref_uv, uint8_t gain);

#endif
