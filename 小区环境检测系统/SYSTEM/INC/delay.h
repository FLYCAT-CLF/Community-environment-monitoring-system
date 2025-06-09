#ifndef __DELAY_H
#define __DELAY_H



void Delay_Init(void);
void delay_us(int nus);
void delay_ms(int nms);
void delay_s(int ns);
void delay_usISR(int nus);
#endif
