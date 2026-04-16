#include "systick.h"
#include "usart_comm.h"
#include "ads124s08.h"
#include "thermocouple.h"
#include "speed_sensor.h"
#include "pcb_sensor.h"
#include <stdio.h>

#define ADS_VREF_UV   2500000.0
#define ADS_GAIN      8
#define CJC_TEMP_C    25.0

// 全局变量，用于毫秒计数
__IO uint32_t systick_ms = 0;

static void send_line(const char *s)
{
    usart0_send_string((uint8_t *)s);
}

int main(void)
{
    uint8_t ch;
    char buf[128];

    systick_config();
    usart0_init(9600);
    ads124s08_init_all();

    send_line("THERMOCOUPLE MODE 12CH\r\n");

    while (1) {
        for (ch = 0; ch < 6; ch++) {
            int32_t code;
            double uv;
            double temp_c;

            ads124s08_set_diff_channel(ADS_DEV_A, ch);
            code = ads124s08_read_data(ADS_DEV_A);
            uv = ads_code_to_uv(code, ADS_VREF_UV, ADS_GAIN);
            temp_c = k_tc_compensated_temp_c(code, CJC_TEMP_C, ADS_VREF_UV, ADS_GAIN);

            snprintf(buf, sizeof(buf),
                     "A%u code=%ld uv=%.2f T=%.2fC\r\n",
                     ch, (long)code, uv, temp_c);
            send_line(buf);
        }

        for (ch = 0; ch < 6; ch++) {
            int32_t code;
            double uv;
            double temp_c;

            ads124s08_set_diff_channel(ADS_DEV_B, ch);
            code = ads124s08_read_data(ADS_DEV_B);
            uv = ads_code_to_uv(code, ADS_VREF_UV, ADS_GAIN);
            temp_c = k_tc_compensated_temp_c(code, CJC_TEMP_C, ADS_VREF_UV, ADS_GAIN);

            snprintf(buf, sizeof(buf),
                     "B%u code=%ld uv=%.2f T=%.2fC\r\n",
                     ch, (long)code, uv, temp_c);
            send_line(buf);
        }

        send_line("\r\n");
        delay_1ms(1000);
    }
}



//#include "gd32f10x.h"
//#include "systick.h"
//#include "usart_comm.h"
//#include "speed_sensor.h"
//#include "pcb_sensor.h"
//#include <stdio.h>

//static void uart_send_line(const char *s)
//{
//    usart0_send_string((uint8_t *)s);
//}

//int main(void)
//{
//    char buf[64];
//    float speed;

//    systick_config();
//    usart0_init(115200);

//    uart_send_line("system start\r\n");

//    speed_sensor_init();
//    pcb_sensor_init();

//    /*
//     * 这里先填一个临时值。
//     * 你后面根据编码轮/输送机构实际标定修改。
//     */
//    speed_set_mm_per_pulse(1.0f);

//    uart_send_line("speed sensor init ok\r\n");
//    uart_send_line("pcb sensor init ok\r\n");

//    while (1) {
//        speed = speed_get_mm_s();

//        snprintf(buf, sizeof(buf), "speed=%.2f mm/s, period=%lu us\r\n",
//                 speed, (unsigned long)speed_get_period_us());
//        uart_send_line(buf);

//        if (pcb_enter_event()) {
//            uart_send_line("PCB ENTER\r\n");
//        }

//        if (pcb_leave_event()) {
//            uart_send_line("PCB LEAVE\r\n");
//        }

//        delay_1ms(500);
//    }
//}

