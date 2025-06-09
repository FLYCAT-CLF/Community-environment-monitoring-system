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
//    GPIOF->MODER|=0x1<<(2*9);//00input01output10复用11analong
//    GPIOF->OTYPER|=0x1<<9;//0PP，1OD
	
		TIM_TimeBaseInitStruct.TIM_Prescaler	= (8400-1); 	//设置预分频器 8400分频 84MHZ/8400 = 10000HZ
		TIM_TimeBaseInitStruct.TIM_Period		= (10000-1); 	//计数周期 重装载值 确定溢出时间：10000/10000HZ = 1s
		TIM_TimeBaseInitStruct.TIM_CounterMode	= TIM_CounterMode_Up; //向上计数
		TIM_TimeBaseInitStruct.TIM_ClockDivision= TIM_CKD_DIV1; //时钟分频与ETR TI有关，配置定时器时忽略即可
		//初始化定时器，配置ARR,PSC。
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseInitStruct);	
	
//		SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//中断分组只有10：8可以
		NVIC->ISER[TIM7_IRQn>>0x5]|=0x1<<(TIM7_IRQn&0x1F);//设置中断0-32，其中的值位中断号
		NVIC->IP[TIM7_IRQn]|=0x6<<4;//高4位有效响中断其中的下标为中断号

//		GPIOF->ODR|=0x1<<9;
		TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	//使能定时器。
		TIM_Cmd(TIM7, ENABLE);
}
void tim3_init(void)
{
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	//打开TIM3硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	
	//TIM3的硬件时钟频率=84MHz/(TIM_Prescaler+1)=84MHz/(8399+1)=10000Hz
	TIM_TimeBaseStructure.TIM_Period = 9;//计数值，用于定时时间的设置，0 ->9共10次计数
	TIM_TimeBaseStructure.TIM_Prescaler = 8399;//预分频值的配置
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//F407没有，不需要配置
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数，就是0 -> TIM_Period，然后就会触发时间更新中断
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	
	/* 定时器时间更新中断使能 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC->ISER[TIM3_IRQn>>0x5]|=0x1<<(TIM3_IRQn&0x1F);//设置中断0-32，其中的值位中断号
	NVIC->IP[TIM3_IRQn]|=0x5<<4;//高4位有效响中断其中的下标为中断号

	//配置NVIC，用于打开TIM3的中断请求通道
//	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	//TIM3的请求通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级0xF
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//响应优先级0xF
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//打开其请求通道
//	NVIC_Init(&NVIC_InitStructure);
	
	
	/* 使能TIM3工作 */
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
