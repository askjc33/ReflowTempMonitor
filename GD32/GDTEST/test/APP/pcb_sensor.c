#include "pcb_sensor.h"

/*
 * PB5: 上拉输入，双边沿触发
 *
 * 逻辑约定：
 * - 当前脚电平为高：认为“挡住/有板”
 * - 当前脚电平为低：认为“未挡住/无板”
 *
 * 如果你实测极性相反，只要把下面两处判断反过来即可。
 */

static volatile uint8_t g_pcb_present = 0;
static volatile uint8_t g_enter_flag = 0;
static volatile uint8_t g_leave_flag = 0;

void pcb_sensor_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* PB5 上拉输入 */
    gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_5);

    /* EXTI5 -> PB5，双边沿触发 */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_5);
    exti_init(EXTI_5, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(EXTI_5);
    nvic_irq_enable(EXTI5_9_IRQn, 2, 1);

    /* 读取初始状态 */
    if (gpio_input_bit_get(GPIOB, GPIO_PIN_5) != RESET) {
        g_pcb_present = 1U;
    } else {
        g_pcb_present = 0U;
    }

    g_enter_flag = 0U;
    g_leave_flag = 0U;
}

uint8_t pcb_is_present(void)
{
    return g_pcb_present;
}

uint8_t pcb_enter_event(void)
{
    if (g_enter_flag) {
        g_enter_flag = 0U;
        return 1U;
    }

    return 0U;
}

uint8_t pcb_leave_event(void)
{
    if (g_leave_flag) {
        g_leave_flag = 0U;
        return 1U;
    }

    return 0U;
}

void EXTI5_9_IRQHandler(void)
{
    if (exti_interrupt_flag_get(EXTI_5) != RESET) {
        if (gpio_input_bit_get(GPIOB, GPIO_PIN_5) != RESET) {
            g_pcb_present = 1U;
            g_enter_flag = 1U;
        } else {
            g_pcb_present = 0U;
            g_leave_flag = 1U;
        }

        exti_interrupt_flag_clear(EXTI_5);
    }
}
