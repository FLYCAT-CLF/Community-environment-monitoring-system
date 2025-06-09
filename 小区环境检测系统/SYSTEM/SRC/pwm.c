#include "stm32f4xx.h"                  // Device header
void pwm_init()
{
		TIM_TimeBaseInitTypeDef		TIM_TimeBaseInitStruct;
		TIM_OCInitTypeDef					TIM_OCInitStruct;
		RCC->AHB1ENR|=0x1<<5;
		RCC->APB1ENR|=0x1<<8;
    GPIOF->MODER|=0x2<<(2*9);//00input01output10����11analong
    GPIOF->OTYPER|=0x1<<9;//0PP��1OD
	 GPIOF->OSPEEDR|=0x2<<(2*9);
		GPIOF->AFR[1]|=GPIO_AF_TIM14<<4;
	
		TIM_TimeBaseInitStruct.TIM_Prescaler	= (84-1); 	//����Ԥ��Ƶ�� 8400��Ƶ 84MHZ/8400 = 10000HZ
		TIM_TimeBaseInitStruct.TIM_Period		= (1000-1); 	//�������� ��װ��ֵ ȷ�����ʱ�䣺10000/10000HZ = 1s
		TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //���ϼ���
		TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //ʱ�ӷ�Ƶ��ETR TI�йأ����ö�ʱ��ʱ���Լ���
		//��ʼ����ʱ��������ARR,PSC��
		TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);	
	
		TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
		TIM_OCInitStruct.TIM_OutputState=1;
		TIM_OCInitStruct.TIM_Pulse=0;
		TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_Low;
		
		TIM_OC1Init(TIM14,&TIM_OCInitStruct);
		TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
		TIM_ARRPreloadConfig(TIM14, ENABLE);
	//ʹ�ܶ�ʱ����
		TIM_Cmd(TIM14, ENABLE);
}

