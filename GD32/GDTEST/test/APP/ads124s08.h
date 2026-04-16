#ifndef ADS124S08_H
#define ADS124S08_H

#include "gd32f10x.h"
#include <stdint.h>

/* 设备选择 */
typedef enum {
    ADS_DEV_A = 0,
    ADS_DEV_B
} ads_dev_t;

/* 命令 */
#define ADS_CMD_RESET       0x06
#define ADS_CMD_WAKEUP      0x02
#define ADS_CMD_RDATA       0x12
#define ADS_CMD_RREG        0x20
#define ADS_CMD_WREG        0x40

/* 寄存器 */
#define ADS_REG_ID          0x00
#define ADS_REG_INPMUX      0x02
#define ADS_REG_PGA         0x03
#define ADS_REG_DATARATE    0x04
#define ADS_REG_REF         0x05

/* API */
void ads124s08_init_all(void);
uint8_t ads124s08_read_reg(ads_dev_t dev, uint8_t reg);
int32_t ads124s08_read_data(ads_dev_t dev);

/* 差分通道选择（0~5） */
void ads124s08_set_diff_channel(ads_dev_t dev, uint8_t ch);

#endif
