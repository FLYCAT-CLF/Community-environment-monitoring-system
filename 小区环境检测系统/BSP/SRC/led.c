#include "led.h"

/************************************
����˵����
LED0������PF9
PF9����͵�ƽ������������ߵ�ƽ������

LED1 -- PF10
LED2 -- PE13
LED3 -- PE14
************************************/

void Led_Init(void)
{	
	RCC->AHB1ENR|=0x1;
	GPIOA->MODER|=0x1<<(2*2);//00input01output10����11analong
	
	//��ȫ��


}
