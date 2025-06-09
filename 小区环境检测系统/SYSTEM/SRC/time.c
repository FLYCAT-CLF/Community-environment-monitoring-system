#include "includes.h"                 
//static RTC_DateTypeDef RTC_DateStruct1;
//static RTC_TimeTypeDef RTC_TimeStruct1;
//SemaphoreHandle_t xMutex1 = NULL;
//char a[50];
void init_time()
{
		TIM_TimeBaseInitTypeDef		TIM_TimeBaseInitStruct;
	
//		RCC->AHB1ENR|=0x1<<5;
		RCC->APB1ENR|=0x1<<5;
//    GPIOF->MODER|=0x1<<(2*9);//00input01output10����11analong
//    GPIOF->OTYPER|=0x1<<9;//0PP��1OD
	
		TIM_TimeBaseInitStruct.TIM_Prescaler	= (8400-1); 	//����Ԥ��Ƶ�� 8400��Ƶ 84MHZ/8400 = 10000HZ
		TIM_TimeBaseInitStruct.TIM_Period		= (10000-1); 	//�������� ��װ��ֵ ȷ�����ʱ�䣺10000/10000HZ = 1s
		TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //���ϼ���
		TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //ʱ�ӷ�Ƶ��ETR TI�йأ����ö�ʱ��ʱ���Լ���
		//��ʼ����ʱ��������ARR,PSC��
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);	
	
//		SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//�жϷ���ֻ��10��8����
		NVIC->ISER[TIM7_IRQn>>0x5]|=0x1<<(TIM7_IRQn&0x1F);//�����ж�0-32�����е�ֵλ�жϺ�
		NVIC->IP[TIM7_IRQn]|=0x6<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�

//		GPIOF->ODR|=0x1<<9;
		TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	//ʹ�ܶ�ʱ����
		TIM_Cmd(TIM7, ENABLE);
}
void tim3_init(void)
{
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	//��TIM3Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//TIM3��Ӳ��ʱ��Ƶ��=84MHz/(TIM_Prescaler+1)=84MHz/(8399+1)=10000Hz
	TIM_TimeBaseStructure.TIM_Period = 9;//����ֵ�����ڶ�ʱʱ������ã�0 ->9��10�μ���
	TIM_TimeBaseStructure.TIM_Prescaler = 8399;//Ԥ��Ƶֵ������
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//F407û�У�����Ҫ����
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���������0 -> TIM_Period��Ȼ��ͻᴥ��ʱ������ж�
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	/* ��ʱ��ʱ������ж�ʹ�� */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC->ISER[TIM3_IRQn>>0x5]|=0x1<<(TIM3_IRQn&0x1F);//�����ж�0-32�����е�ֵλ�жϺ�
	NVIC->IP[TIM3_IRQn]|=0x5<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�

	//����NVIC�����ڴ�TIM3���ж�����ͨ��
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//TIM3������ͨ��
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//��ռ���ȼ�0xF
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//��Ӧ���ȼ�0xF
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//��������ͨ��
//	NVIC_Init(&NVIC_InitStructure);
	
	
	/* ʹ��TIM3���� */
	TIM_Cmd(TIM3, ENABLE);	
}
//void TIM7_IRQHandler(void)
//{
//	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//	long s=0;
//	if(TIM7->SR&0x1)
//	{
//		RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct1);
//		RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct1);
//	
//		snprintf(a,sizeof(a),"Date:20%d.%d.%d,TIME:%d:%d:%d",RTC_DateStruct1.RTC_Year,\
//		RTC_DateStruct1.RTC_Month,RTC_DateStruct1.RTC_Date,\
//		RTC_TimeStruct1.RTC_Hours,RTC_TimeStruct1.RTC_Minutes,RTC_TimeStruct1.RTC_Seconds);

//		if(xSemaphoreGiveFromISR(g_sem_binary, &xHigherPriorityTaskWoken) == pdTRUE)
//		{
//				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//		}
////		xSemaphoreTakeFromISR(g_sem_binary,NULL);

////		OLED_ShowStr(0,0,a,1);
////		OLED_ShowStr(0,0,"123",1);
////		printf("%s",a);
////		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//		TIM7->SR&=0;
//	}
//}
