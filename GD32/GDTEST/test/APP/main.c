#include "systick.h"
#include "usart_comm.h"
#include "ads124s08.h"
#include "thermocouple.h"
#include "speed_sensor.h"
#include "pcb_sensor.h"
#include <stdio.h>
#include <string.h>

/************************ 配置参数 ************************/
#define ADS_VREF_UV             2500000.0
#define ADS_GAIN                8
#define CJC_TEMP_C              25.0
#define DATA_UPDATE_INTERVAL_MS 100

#define ZONE_COUNT              12
#define MAX_BARCODE_LEN         16
#define POS_STEP_MM             100.0f

/************************ PCB结构（关键改造） ************************/
typedef struct
{
    uint16_t id;
    float remain_mm;   // 距离进入下一温区的剩余距离
} pcb_node_t;

/************************ 全局变量 ************************/
__IO uint32_t systick_ms = 0;

static uint32_t g_seq = 1000;
static float g_temps[ZONE_COUNT] = {0};

/* 原输出数组（协议用） */
static uint16_t g_zone_board_ids[ZONE_COUNT] = {0};

/* 新模型：每个温区一个PCB节点 */
static pcb_node_t g_pcb_nodes[ZONE_COUNT];

static float g_speed_mm_s = 0.0f;
static uint16_t g_next_board_id = 1;

/************************ 基础函数 ************************/
static void send_string(const char *s)
{
    usart0_send_string((uint8_t *)s);
}

static void make_barcode(uint16_t id, char *barcode)
{
    sprintf(barcode, "PCB%03u", id);
}

/************************ 温度采集 ************************/
static void collect_12ch_temperature(float temps[ZONE_COUNT])
{
    uint8_t ch;

    for (ch = 0; ch < 6; ch++)
    {
        int32_t code = ads124s08_read_data(ADS_DEV_A);
        temps[ch] = (float)k_tc_compensated_temp_c(code, CJC_TEMP_C, ADS_VREF_UV, ADS_GAIN);
    }

    for (ch = 0; ch < 6; ch++)
    {
        int32_t code = ads124s08_read_data(ADS_DEV_B);
        temps[ch + 6] = (float)k_tc_compensated_temp_c(code, CJC_TEMP_C, ADS_VREF_UV, ADS_GAIN);
    }
}

/************************ PCB位置更新（核心重构） ************************/
static void update_pcb_position(void)
{
    uint8_t i;
    float move;

    /* 1 进板 */
    if (pcb_enter_event())
    {
        if (g_pcb_nodes[0].id == 0)
        {
            g_pcb_nodes[0].id = g_next_board_id++;
            g_pcb_nodes[0].remain_mm = POS_STEP_MM;

            if (g_next_board_id > 999)
                g_next_board_id = 1;
        }
    }

    /* 计算本周期位移 */
    move = g_speed_mm_s * (DATA_UPDATE_INTERVAL_MS / 1000.0f);

    /* 2 所有PCB独立移动（关键） */
    for (i = 0; i < ZONE_COUNT; i++)
    {
        if (g_pcb_nodes[i].id != 0)
        {
            g_pcb_nodes[i].remain_mm -= move;
        }
    }
			/* 3 先处理最后一个温区（必须在推进前） */
			if (g_pcb_nodes[ZONE_COUNT - 1].id != 0 &&
					g_pcb_nodes[ZONE_COUNT - 1].remain_mm <= 0)
			{
					g_pcb_nodes[ZONE_COUNT - 1].id = 0;
			}
    /* 4 推进（逐个判断） */
    for (i = ZONE_COUNT - 1; i > 0; i--)
    {
        if (g_pcb_nodes[i - 1].id != 0 &&
            g_pcb_nodes[i - 1].remain_mm <= 0)
        {
            /* 移动到下一温区 */
            g_pcb_nodes[i] = g_pcb_nodes[i - 1];
            g_pcb_nodes[i].remain_mm = POS_STEP_MM;

            /* 清空原位置 */
            g_pcb_nodes[i - 1].id = 0;
        }
    }

    /* 5 同步到原数组（协议输出） */
    for (i = 0; i < ZONE_COUNT; i++)
    {
        g_zone_board_ids[i] = g_pcb_nodes[i].id;
    }
}

/************************ 发送数据 ************************/
static void send_protocol_packet(void)
{
    char tx_buf[512];
    char barcode[MAX_BARCODE_LEN];
    int len = 0;
    int i;

    g_seq++;

    /* TEMP */
    len += sprintf(&tx_buf[len], "TEMP,%lu", (unsigned long)g_seq);
    for (i = 0; i < ZONE_COUNT; i++)
    {
        len += sprintf(&tx_buf[len], ",%.1f", g_temps[i]);
    }
    len += sprintf(&tx_buf[len], "\r\n");

    /* POS */
    len += sprintf(&tx_buf[len], "POS,%lu", (unsigned long)g_seq);
    for (i = 0; i < ZONE_COUNT; i++)
    {
        if (g_zone_board_ids[i] == 0)
        {
            len += sprintf(&tx_buf[len], ",EMPTY");
        }
        else
        {
            make_barcode(g_zone_board_ids[i], barcode);
            len += sprintf(&tx_buf[len], ",%s", barcode);
        }
    }
    len += sprintf(&tx_buf[len], "\r\n");

    /* SPEED */
    len += sprintf(&tx_buf[len], "SPEED,%.1f\r\n", g_speed_mm_s);

    send_string(tx_buf);
}

/************************ 主函数 ************************/
int main(void)
{
    systick_config();
    usart0_init(9600);
    ads124s08_init_all();
    speed_sensor_init();
    pcb_sensor_init();

    speed_set_mm_per_pulse(1.0f);

    send_string("ReflowTempMonitor Stable Start\r\n");

    while (1)
    {
        collect_12ch_temperature(g_temps);

        /* 实际运行请打开 */
        //g_speed_mm_s = speed_get_mm_s();

        /* 测试用（可删） */
         g_speed_mm_s = 43.5f;

        update_pcb_position();

        send_protocol_packet();

        delay_1ms(DATA_UPDATE_INTERVAL_MS);
    }
}