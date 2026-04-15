#include "usart_comm.h"

/**
*@brief: usart0的初始化
*@param baudval： 波特率
*@retval NONE
*/
void usart0_init(uint32_t baudval){
	/* 初始化时钟和对应的io口 */
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_USART0);
	
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	
	// 配置usart0的工作参数
	usart_deinit(USART0);
	usart_baudrate_set(USART0, baudval);        //波特率
	usart_parity_config(USART0, USART_PM_NONE); //无奇偶校验
	usart_word_length_set(USART0, USART_WL_8BIT); //bit长度
	usart_stop_bit_set(USART0, USART_STB_1BIT);   //停止位
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE); //使能发送
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);   //使能接收
	
	usart_enable(USART0);
}


/**
*@brief: usart0发送一个字节
*@param ch：待发送字节
*@retval NONE
*/
void usart0_send_byte(uint8_t ch){
	usart_data_transmit(USART0, ch);
	
	while(usart_flag_get(USART0, USART_FLAG_TBE)==RESET);
}

/**
*@brief: usart0发送字符串
*@param ch：待发送字符串指针
*@retval NONE
*/
void usart0_send_string(uint8_t *ch){
	uint32_t k = 0;
	while(*(ch+k) != '\0'){
		usart0_send_byte(*(ch+k));
		k++;
	}
}
