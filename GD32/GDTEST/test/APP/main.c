#include "systick.h"
#include "usart_comm.h"
#include "ads124s08.h"
#include <stdio.h>

static void send_int(int32_t v)
{
    char buf[20];
    sprintf(buf, "%ld", v);
    usart0_send_string((uint8_t*)buf);
}

int main(void)
{
    uint8_t ch;

    systick_config();
    usart0_init(9600);

    ads124s08_init_all();

    usart0_send_string((uint8_t*)"DIFF MODE 12CH\r\n");

    while (1)
    {
        /* ===== ADS A£º6Â· ===== */
        for (ch = 0; ch < 6; ch++)
        {
            int32_t val;

            ads124s08_set_diff_channel(ADS_DEV_A, ch);
            val = ads124s08_read_data(ADS_DEV_A);

            usart0_send_string((uint8_t*)"A");
            usart0_send_byte('0' + ch);
            usart0_send_string((uint8_t*)"=");

            send_int(val);
            usart0_send_string((uint8_t*)"  ");
        }

        usart0_send_string((uint8_t*)"\r\n");

        /* ===== ADS B£º6Â· ===== */
        for (ch = 0; ch < 6; ch++)
        {
            int32_t val;

            ads124s08_set_diff_channel(ADS_DEV_B, ch);
            val = ads124s08_read_data(ADS_DEV_B);

            usart0_send_string((uint8_t*)"B");
            usart0_send_byte('0' + ch);
            usart0_send_string((uint8_t*)"=");

            send_int(val);
            usart0_send_string((uint8_t*)"  ");
        }

        usart0_send_string((uint8_t*)"\r\n\r\n");

        delay_1ms(500);
    }
}