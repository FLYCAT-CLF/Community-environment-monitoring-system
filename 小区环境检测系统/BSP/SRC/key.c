#include "stm32f4xx.h"                  // Device header
void init_key()
{
    RCC->AHB1ENR|=0x3<<4;
		RCC->APB2ENR|=0x1<<14;
    GPIOF->MODER|=0x1<<(2*9);//00input01output10复用11analong
    GPIOF->OTYPER|=0x1<<9;//0PP，1OD
    SYSCFG->EXTICR[0]|=0x1<<10;
		EXTI->IMR|=0x1<<2;
		EXTI->EMR|=0x1<<2;
		EXTI->RTSR|=0x1<<2;//下降沿
//		SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//中断分组只有10：8可以
		NVIC->ISER[0]|=0x1<<8;//设置中断0-32，其中的值位中断号
		NVIC->IP[8]|=0x5<<4;//高4位有效响中断其中的下标为中断号
		GPIOF->ODR|=0x1<<9;

}
void EXTI2_IRQHandler(void)
{
	if(EXTI->PR&(0x1<<2))
	{
		GPIOF->ODR^=0x1<<9;
		EXTI->PR|=0x1<<2;
	}
}
