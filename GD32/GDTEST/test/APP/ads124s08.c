#include "ads124s08.h"
#include "systick.h"

/* ================= 引脚定义 ================= */

/* SPI */
#define SCLK_PORT   GPIOB
#define SCLK_PIN    GPIO_PIN_0

#define DIN_PORT    GPIOB
#define DIN_PIN     GPIO_PIN_1

#define DOUT_PORT   GPIOA
#define DOUT_PIN    GPIO_PIN_7

/* ADS A */
#define CS_A_PORT   GPIOB
#define CS_A_PIN    GPIO_PIN_10
#define DRDY_A_PORT GPIOB
#define DRDY_A_PIN  GPIO_PIN_2
#define RESET_A_PORT GPIOA
#define RESET_A_PIN  GPIO_PIN_3

/* ADS B */
#define CS_B_PORT   GPIOA
#define CS_B_PIN    GPIO_PIN_5
#define DRDY_B_PORT GPIOA
#define DRDY_B_PIN  GPIO_PIN_6
#define RESET_B_PORT GPIOB
#define RESET_B_PIN  GPIO_PIN_12

/* ================= GPIO 宏 ================= */

#define SCLK_H() gpio_bit_set(SCLK_PORT, SCLK_PIN)
#define SCLK_L() gpio_bit_reset(SCLK_PORT, SCLK_PIN)

#define DIN_H()  gpio_bit_set(DIN_PORT, DIN_PIN)
#define DIN_L()  gpio_bit_reset(DIN_PORT, DIN_PIN)

#define DOUT_READ() gpio_input_bit_get(DOUT_PORT, DOUT_PIN)

/* ================= 内部函数 ================= */

static void cs_low(ads_dev_t dev)
{
    if (dev == ADS_DEV_A)
        gpio_bit_reset(CS_A_PORT, CS_A_PIN);
    else
        gpio_bit_reset(CS_B_PORT, CS_B_PIN);
}

static void cs_high(ads_dev_t dev)
{
    if (dev == ADS_DEV_A)
        gpio_bit_set(CS_A_PORT, CS_A_PIN);
    else
        gpio_bit_set(CS_B_PORT, CS_B_PIN);
}

static uint8_t drdy_ready(ads_dev_t dev)
{
    if (dev == ADS_DEV_A)
        return gpio_input_bit_get(DRDY_A_PORT, DRDY_A_PIN) == RESET;
    else
        return gpio_input_bit_get(DRDY_B_PORT, DRDY_B_PIN) == RESET;
}

static void reset_hw(ads_dev_t dev)
{
    if (dev == ADS_DEV_A) {
        gpio_bit_reset(RESET_A_PORT, RESET_A_PIN);
        delay_1ms(2);
        gpio_bit_set(RESET_A_PORT, RESET_A_PIN);
    } else {
        gpio_bit_reset(RESET_B_PORT, RESET_B_PIN);
        delay_1ms(2);
        gpio_bit_set(RESET_B_PORT, RESET_B_PIN);
    }
    delay_1ms(5);
}

/* 软件 SPI */
static uint8_t spi_rw(uint8_t tx)
{
    uint8_t i, rx = 0;

    for (i = 0; i < 8; i++) {
        SCLK_L();

        if (tx & 0x80) DIN_H(); else DIN_L();

        __NOP();__NOP();

        SCLK_H();

        rx <<= 1;
        if (DOUT_READ()) rx |= 1;

        __NOP();__NOP();

        tx <<= 1;
    }

    SCLK_L();
    return rx;
}

static void send_cmd(ads_dev_t dev, uint8_t cmd)
{
    cs_low(dev);
    spi_rw(cmd);
    cs_high(dev);
    delay_1ms(1);
}

uint8_t ads124s08_read_reg(ads_dev_t dev, uint8_t reg)
{
    uint8_t val;

    cs_low(dev);
    spi_rw(ADS_CMD_RREG | reg);
    spi_rw(0x00);
    val = spi_rw(0xFF);
    cs_high(dev);

    return val;
}

static void write_reg(ads_dev_t dev, uint8_t reg, uint8_t val)
{
    cs_low(dev);
    spi_rw(ADS_CMD_WREG | reg);
    spi_rw(0x00);
    spi_rw(val);
    cs_high(dev);
}

/* ================= 初始化 ================= */

void ads124s08_init_all(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* SPI */
    gpio_init(SCLK_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SCLK_PIN);
    gpio_init(DIN_PORT,  GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, DIN_PIN);
    gpio_init(DOUT_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, DOUT_PIN);

    /* A */
    gpio_init(CS_A_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, CS_A_PIN);
    gpio_init(RESET_A_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET_A_PIN);
    gpio_init(DRDY_A_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, DRDY_A_PIN);

    /* B */
    gpio_init(CS_B_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, CS_B_PIN);
    gpio_init(RESET_B_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RESET_B_PIN);
    gpio_init(DRDY_B_PORT, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, DRDY_B_PIN);

    cs_high(ADS_DEV_A);
    cs_high(ADS_DEV_B);

    /* A 初始化 */
    reset_hw(ADS_DEV_A);
    send_cmd(ADS_DEV_A, ADS_CMD_RESET);
    send_cmd(ADS_DEV_A, ADS_CMD_WAKEUP);
		send_cmd(ADS_DEV_A, 0x08);
		
    write_reg(ADS_DEV_A, ADS_REG_PGA, 0x03);       // 增益8
    write_reg(ADS_DEV_A, ADS_REG_DATARATE, 0x06);
    write_reg(ADS_DEV_A, ADS_REG_REF, 0x0A);

    /* B 初始化 */
    reset_hw(ADS_DEV_B);
    send_cmd(ADS_DEV_B, ADS_CMD_RESET);
    send_cmd(ADS_DEV_B, ADS_CMD_WAKEUP);
		send_cmd(ADS_DEV_B, 0x08);
		
    write_reg(ADS_DEV_B, ADS_REG_PGA, 0x03);
    write_reg(ADS_DEV_B, ADS_REG_DATARATE, 0x06);
    write_reg(ADS_DEV_B, ADS_REG_REF, 0x0A);
}

/* ================= 差分通道 ================= */

void ads124s08_set_diff_channel(ads_dev_t dev, uint8_t ch)
{
    uint8_t ainp = ch * 2 + 1;
    uint8_t ainm = ch * 2;

    write_reg(dev, ADS_REG_INPMUX, (ainp << 4) | ainm);

    delay_1ms(2);
}

/* ================= 读数据 ================= */

int32_t ads124s08_read_data(ads_dev_t dev)
{
    uint8_t b0, b1, b2;
    int32_t val;

    while (!drdy_ready(dev));

    cs_low(dev);
    spi_rw(ADS_CMD_RDATA);
    b0 = spi_rw(0xFF);
    b1 = spi_rw(0xFF);
    b2 = spi_rw(0xFF);
    cs_high(dev);

    val = ((int32_t)b0 << 16) | ((int32_t)b1 << 8) | b2;

    if (val & 0x800000)
        val |= 0xFF000000;

    return val;
}
