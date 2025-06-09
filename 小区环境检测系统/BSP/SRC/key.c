#include "stm32f4xx.h"                  // Device header
void init_key()
{
    RCC->AHB1ENR|=0x3<<4;
		RCC->APB2ENR|=0x1<<14;
    GPIOF->MODER|=0x1<<(2*9);//00input01output10����11analong
    GPIOF->OTYPER|=0x1<<9;//0PP��1OD
    SYSCFG->EXTICR[0]|=0x1<<10;
		EXTI->IMR|=0x1<<2;
		EXTI->EMR|=0x1<<2;
		EXTI->RTSR|=0x1<<2;//�½���
//		SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//�жϷ���ֻ��10��8����
		NVIC->ISER[0]|=0x1<<8;//�����ж�0-32�����е�ֵλ�жϺ�
		NVIC->IP[8]|=0x5<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�
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
