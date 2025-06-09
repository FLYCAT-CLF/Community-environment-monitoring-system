#include "IIC.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>
void IIC_init(void)
{
	RCC->AHB1ENR|=0x1<<1;
	GPIOB->MODER|=0x5<<(2*8);
	GPIOB->OTYPER|=0x3<<8;
	GPIOB->PUPDR|=0x5<<(2*8);
//	GPIOB->OSPEEDR|=0xA<<(2*8);
}

void IIC_Start(void)
{
	//数据为输出
//	Iic_Sda_Mode(GPIO_Mode_OUT);
//	GPIOA->MODER&=0;
	GPIOB->MODER|=0x1<<(2*9);
	
	SDA_OUT = 1;
	SCL = 1;
	delay_us(5);
	
	SDA_OUT = 0;
	
	delay_us(5);
	
	SCL = 0;

}
void IIC_Stop(void)
{
	//数据为输出
//	Iic_Sda_Mode(GPIO_Mode_OUT);
//	GPIOA->MODER&=0;
	GPIOB->MODER|=0x1<<(2*9);
	SCL = 0;
	SDA_OUT = 0;
	
	delay_us(5);
	
	SCL = 1;
	
	delay_us(5);
	
	SDA_OUT = 1;

}

//发送一位数据，数据转换为引脚电平
//Ack = 1:引脚为高电平  Ack=0，引脚为低电平
void IIC_Send_Ack(u8 Ack)
{
	//数据为输出
//	Iic_Sda_Mode(GPIO_Mode_OUT);
GPIOB->MODER|=0x1<<(2*9);

	SCL = 0;
	
	//在时钟为低电平时，可变更引脚状态
	if(Ack == 1)
		SDA_OUT = 1;
	else
		SDA_OUT = 0;
	
	delay_us(5);
	
	SCL = 1;
	
	delay_us(5);
	
	SCL = 0;

}


//发送一个字节（一个字节8位）
//先发高位数据:1001 1000
void IIC_Send_Byte(u8 txdata)
{
	u8 i;
	//数据为输出
//	Iic_Sda_Mode(GPIO_Mode_OUT);
//	GPIOA->MODER&=0;
	GPIOB->MODER|=0x1<<(2*9);
	//8位要8个脉冲
	
	SCL = 0;
	
	for(i=0; i<8; i++)
	{
		//在时钟为低电平时，可变更引脚状态
		if( txdata & (1<<(7-i)))
			SDA_OUT = 1;
		else
			SDA_OUT = 0;
		
		delay_us(5);
		
		SCL = 1;
		
		delay_us(5);
		
		SCL = 0;	
	
	
	}
	

}
u8 IIC_Read_Byte(void)
{
	u8 i,s=0;
	GPIOB->MODER&=~(0x3<<9);
	//8位要8个脉冲
	SCL = 0;
	
	for(i=0; i<8; i++)
	{
		delay_us(5);
		SCL = 1;
		delay_us(5);
		//在时钟为低电平时，可变更引脚状态
		if((GPIOA->IDR>>9)&1)
			s|=1<<(7-i);
		SCL = 0;
	}
return	s;
}
//接受一位数据,判断引脚电平
u8 IIC_Recv_Ack(void)
{
	u8 ack = 0;
	
	//数据为输入
GPIOB->MODER&=~(0x3<<2*9);

	SCL = 0;
	
	delay_us(5);

	//在时钟线为高电平时，判断引脚数据，接受数据	
	SCL = 1;
	
	delay_us(5);
	
	if(SDA_IN)
		ack = 1;
	else
		ack = 0;	

	SCL = 0;	

	return ack;
}

