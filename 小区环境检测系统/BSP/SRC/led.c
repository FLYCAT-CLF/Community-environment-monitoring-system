#include "led.h"

/************************************
引脚说明：
LED0连接在PF9
PF9输出低电平，灯亮；输出高电平，灯灭

LED1 -- PF10
LED2 -- PE13
LED3 -- PE14
************************************/

void Led_Init(void)
{	
	RCC->AHB1ENR|=0x1;
	GPIOA->MODER|=0x1<<(2*2);//00input01output10复用11analong
	
	//灯全灭


}
