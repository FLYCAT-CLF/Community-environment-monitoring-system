#include "stm32f4xx.h"                  // Device header
void pwm_init()
{
		TIM_TimeBaseInitTypeDef		TIM_TimeBaseInitStruct;
		TIM_OCInitTypeDef					TIM_OCInitStruct;
		RCC->AHB1ENR|=0x1<<5;
		RCC->APB1ENR|=0x1<<8;
    GPIOF->MODER|=0x2<<(2*9);//00input01output10复用11analong
    GPIOF->OTYPER|=0x1<<9;//0PP，1OD
	 GPIOF->OSPEEDR|=0x2<<(2*9);
		GPIOF->AFR[1]|=GPIO_AF_TIM14<<4;
	
		TIM_TimeBaseInitStruct.TIM_Prescaler	= (84-1); 	//设置预分频器 8400分频 84MHZ/8400 = 10000HZ
		TIM_TimeBaseInitStruct.TIM_Period		= (1000-1); 	//计数周期 重装载值 确定溢出时间：10000/10000HZ = 1s
		TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //向上计数
		TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //时钟分频与ETR TI有关，配置定时器时忽略即可
		//初始化定时器，配置ARR,PSC。
		TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);	
	
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
		TIM_OCInitStruct.TIM_OutputState=1;
		TIM_OCInitStruct.TIM_Pulse=0;
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_Low;
		
		TIM_OC1Init(TIM14,&TIM_OCInitStruct);
		TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM14, ENABLE);
	//使能定时器。
		TIM_Cmd(TIM14, ENABLE);
}

