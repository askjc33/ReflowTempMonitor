#include "thermocouple.h"

/*
 * ADC 原始码 -> 微伏
 * ADS124S08: 24-bit signed, FS ~= +/- Vref / Gain
 */
double ads_code_to_uv(int32_t code, double vref_uv, uint8_t gain)
{
    return ((double)code * vref_uv) / ((double)gain * 8388608.0);
}

/*
 * K型热电偶：
 * 先用工程近似，0~100C附近约 41 uV/C
 * 用于冷端补偿的初版足够把系统跑起来
 */
double k_cjc_temp_to_uv(double temp_c)
{
    return temp_c * 41.0;
}

/*
 * K型热电偶电压 -> 温度
 * 初版工程近似：41 uV/C
 * 后续可替换为 NIST 多项式
 */
double k_uv_to_temp_c(double uv)
{
    return uv / 41.0;
}

/*
 * 带冷端补偿的温度
 * code      : ADC原始码
 * cjc_temp_c: 冷端温度（板端温度）
 * vref_uv   : 参考电压，内部2.5V填 2500000.0
 * gain      : PGA增益，比如 8
 */
double k_tc_compensated_temp_c(int32_t code, double cjc_temp_c, double vref_uv, uint8_t gain)
{
    double tc_uv;
    double cjc_uv;
    double total_uv;

    tc_uv = ads_code_to_uv(code, vref_uv, gain);
    cjc_uv = k_cjc_temp_to_uv(cjc_temp_c);
    total_uv = tc_uv + cjc_uv;

    return k_uv_to_temp_c(total_uv);
}
