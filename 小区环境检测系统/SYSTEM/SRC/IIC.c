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
	//����Ϊ���
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
	//����Ϊ���
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

//����һλ���ݣ�����ת��Ϊ���ŵ�ƽ
//Ack = 1:����Ϊ�ߵ�ƽ  Ack=0������Ϊ�͵�ƽ
void IIC_Send_Ack(u8 Ack)
{
	//����Ϊ���
//	Iic_Sda_Mode(GPIO_Mode_OUT);
GPIOB->MODER|=0x1<<(2*9);

	SCL = 0;
	
	//��ʱ��Ϊ�͵�ƽʱ���ɱ������״̬
	if(Ack == 1)
		SDA_OUT = 1;
	else
		SDA_OUT = 0;
	
	delay_us(5);
	
	SCL = 1;
	
	delay_us(5);
	
	SCL = 0;

}


//����һ���ֽڣ�һ���ֽ�8λ��
//�ȷ���λ����:1001 1000
void IIC_Send_Byte(u8 txdata)
{
	u8 i;
	//����Ϊ���
//	Iic_Sda_Mode(GPIO_Mode_OUT);
//	GPIOA->MODER&=0;
	GPIOB->MODER|=0x1<<(2*9);
	//8λҪ8������
	
	SCL = 0;
	
	for(i=0; i<8; i++)
	{
		//��ʱ��Ϊ�͵�ƽʱ���ɱ������״̬
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
	//8λҪ8������
	SCL = 0;
	
	for(i=0; i<8; i++)
	{
		delay_us(5);
		SCL = 1;
		delay_us(5);
		//��ʱ��Ϊ�͵�ƽʱ���ɱ������״̬
		if((GPIOA->IDR>>9)&1)
			s|=1<<(7-i);
		SCL = 0;
	}
return	s;
}
//����һλ����,�ж����ŵ�ƽ
u8 IIC_Recv_Ack(void)
{
	u8 ack = 0;
	
	//����Ϊ����
GPIOB->MODER&=~(0x3<<2*9);

	SCL = 0;
	
	delay_us(5);

	//��ʱ����Ϊ�ߵ�ƽʱ���ж��������ݣ���������	
	SCL = 1;
	
	delay_us(5);
	
	if(SDA_IN)
		ack = 1;
	else
		ack = 0;	

	SCL = 0;	

	return ack;
}

