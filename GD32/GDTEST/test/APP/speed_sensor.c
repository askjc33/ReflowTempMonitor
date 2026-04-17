#include "speed_sensor.h"

/*
 * PB4: 上拉输入，上升沿触发
 * 使用 TIMER2 作为 1MHz 自由运行计数器
 *
 * 注意：
 * PB4 在 GD32F103 上默认可能被 JTAG 占用，
 * 这里会关闭 JTAG，保留 SWD。
 */

static volatile uint32_t g_last_tick = 0;
static volatile uint32_t g_period_us = 0;
static volatile uint8_t  g_speed_valid = 0;
static volatile uint8_t speed_updated_flag = 0;

static float g_mm_per_pulse = 1.0f;

#define TIMER_MAX_COUNT  0xFFFFU

void speed_sensor_init(void)
{
    /* 时钟 */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER2);

    /* 关闭 JTAG，保留 SWD，释放 PB4 */
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    /* PB4 上拉输入 */
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    /* EXTI4 -> PB4，上升沿触发 */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_4);
    exti_init(EXTI_4, EXTI_INTERRUPT, EXTI_TRIG_RISING);
    exti_interrupt_flag_clear(EXTI_4);
    nvic_irq_enable(EXTI4_IRQn, 2, 0);

    /*
     * TIMER2 1MHz:
     * 72MHz / 72 = 1MHz
     * 16位计数器，1 tick = 1us
     */
    timer_deinit(TIMER2);
    timer_prescaler_config(TIMER2, 71, TIMER_PSC_RELOAD_NOW);
    timer_autoreload_value_config(TIMER2, TIMER_MAX_COUNT);
    timer_counter_value_config(TIMER2, 0);
    timer_enable(TIMER2);

    g_last_tick = 0;
    g_period_us = 0;
    g_speed_valid = 0;
}

void speed_set_mm_per_pulse(float val)
{
    g_mm_per_pulse = val;
}

uint8_t speed_is_valid(void)
{
    return g_speed_valid;
}

uint32_t speed_get_period_us(void)
{
    return g_period_us;
}

float speed_get_mm_s(void)
{
    uint32_t period;

    if (!g_speed_valid) {
        return 0.0f;
    }

    period = g_period_us;
    if (period == 0U) {
        return 0.0f;
    }

    return (g_mm_per_pulse * 1000000.0f) / (float)period;
}

void EXTI4_IRQHandler(void)
{
    if (exti_interrupt_flag_get(EXTI_4) != RESET) {
        uint32_t now = timer_counter_read(TIMER2);

        if (g_speed_valid) {
            if (now >= g_last_tick) {
                g_period_us = now - g_last_tick;
            } else {
                g_period_us = (TIMER_MAX_COUNT - g_last_tick + 1U) + now;
            }
        }

        g_last_tick = now;
        g_speed_valid = 1U;
        speed_updated_flag = 1;
        exti_interrupt_flag_clear(EXTI_4);
    }
}

uint8_t speed_get_flag(void)
{
    uint8_t tmp = speed_updated_flag;
    speed_updated_flag = 0;  // 读走自动清除
    return tmp;
}
