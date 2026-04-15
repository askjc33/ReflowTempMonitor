#ifndef USART_COMM_H
#define USART_COMM_H
/**
@brief: 串口处理的模块
*/

#include "gd32f10x.h"

void usart0_init(uint32_t baudval);

void usart0_send_byte(uint8_t ch);

void usart0_send_string(uint8_t *ch);

void usart0_send_uint(uint32_t i_var);

#endif
