#ifndef __USART_H_
#define __USART_H_
#include "stm32f4xx.h" 

void usart1_init(u32 myBaudRate);
void usart1_send_bytes(uint8_t *buf,uint32_t len);
void usart3_init(u32 myBaudRate);
void usart3_send_str(const char *pbuf);
#endif
