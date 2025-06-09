#ifndef __IIC_H
#define __IIC_H

#include "stm32f4xx.h"
#define SCL  	PBout(8)
#define SDA_IN	PBin(9)
#define SDA_OUT	PBout(9)

void IIC_init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Recv_Ack(void);
void IIC_Send_Ack(u8 Ack);
void IIC_Send_Byte(u8 txdata);
u8 IIC_Read_Byte(void);
#endif
