#include "dht11.h"


/**********************************
引脚说明：

PG9 -- DQ(DHT11)
***********************************/

void Dht11_Init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//第9号引脚
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//输出模式
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_25MHz;			//引脚的速度最大为25MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//没有使用内部上拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	//温湿度模块还没有工作，那么它的触发引脚是高电平
	PGout(9) = 1;
}

//引脚模式变更
void Dht11_Pin_Mode(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;				//第9号引脚
	GPIO_InitStructure.GPIO_Mode  = mode;					//输入/输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增强驱动能力，引脚的输出电流更大
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;		//引脚的速度最大为100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;		//没有使用内部上拉电阻
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
}

//启动DHT11
int Dht11_Start(void)
{
	u16 t = 0;
	
	
	//启动信号 
	//做为输出模式
	Dht11_Pin_Mode(GPIO_Mode_OUT);
	PGout(9) = 1;
	delay_ms(2);
	PGout(9) = 0;
	delay_ms(20);
	PGout(9) = 1;
	delay_us(30);
	
	//回响信号 
	//做为输入模式
	Dht11_Pin_Mode(GPIO_Mode_IN);	
	//判断回响信号有低与高电平即可
	
	
	//下面是比较简单的过滤回响信号的代码，下面代码，如果硬件有坏，有可以出现卡死现象
	//while(PGin(9) == 1);
	//while(PGin(9) == 0);
	//while(PGin(9) == 1);
	
	
	
	//等待低电平的到来
	while(PGin(9) == 1)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //等待200us未有低电平到来，则返回
			return -1;
	}
	//延时40us，再过滤低电平
	delay_us(40);
	
	t = 0;
	//等待高电平的到来,过滤低电平
	while(PGin(9) == 0)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //等待200us未有高电平到来，则返回
			return -1;
	}	
	
	//延时40us
	delay_us(40);	
	
	t = 0;
	//等待低电平的到来，过滤高电平
	while(PGin(9) == 1)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //等待200us未有低电平到来，则返回
			return -1;
	}	
	
	//执行到这里，接下来就是DHT11传输40位数据
	
	return 0;
	

}

//接受一个字节
u8 Dht11_Recv_Byte(u8 *s)
{
	u16 t = 0;
	for(int a=0;a<5;a++)
	for(u8 i=0; i<8; i++)
	{
		t = 0;
		//等待高电平的到来,过滤低电平
		while(PGin(9) == 0)
		{
			t++;
			delay_us(2);
			
			if(t >= 100) //等待200us未有高电平到来，则返回
				return 0;
		}	
			
		//程序来到这里，说明是高电平
		delay_us(40);
//		TIM3->CR1 |= TIM_CR1_CEN;
		if(PGin(9))
		{
			//先发高位，先合成高位
			*(s+a) |= (1<<(7-i));
			
			//还有大概30us高电平
			t = 0;
			//等待低电平的到来，过滤高电平
			while(PGin(9) == 1)
			{
				t++;
				delay_us(2);
				if(t >= 100) //等待200us未有低电平到来，则返回
					return 0;
			}			
		}
	}
	return 1;
	
}




