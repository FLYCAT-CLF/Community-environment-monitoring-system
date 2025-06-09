#include "includes.h"
#define  BKP  0x2501
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;

void rtc_init(void)
{
	RTC_InitTypeDef		RTC_InitStruct;
	RTC_TimeTypeDef		RTC_TimeStruct1;
	RTC_DateTypeDef	  RTC_DateStruct1;
	u8 i,i1,i2,i3;
	RCC->APB1ENR|=RCC_APB1Periph_PWR;
	PWR_BackupAccessCmd(ENABLE);
	
		if(RTC_ReadBackupRegister(RTC_BKP_DR0) != BKP) 
	{
		//配置RTC时钟源，使能RTC时钟：
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//如果使用LSE，要打开LSE：
		RCC_LSEConfig(RCC_LSE_ON);
		
		//等待时钟源稳定
		//方法1
		//delay_ms(20);
		/* Wait till LSE is ready */  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			
		}
		RTC_InitStruct.RTC_HourFormat	= RTC_HourFormat_24; //小时制选择
		RTC_InitStruct.RTC_AsynchPrediv	= 0x7F;		//异步预分频器 128分频
		RTC_InitStruct.RTC_SynchPrediv	= 0xFF;		//同步预分频器 256分频
		//初始化RTC(同步/异步分频系数和时钟格式)：
		RTC_Init(&RTC_InitStruct);
		
		RTC_TimeStruct1.RTC_H12		= RTC_H12_PM; //下午，在24小时制时，这个参数是无用
		RTC_TimeStruct1.RTC_Hours	= 12; //时
		RTC_TimeStruct1.RTC_Minutes	= 0; //分
		RTC_TimeStruct1.RTC_Seconds	= 0; //秒
		//RTC_Format_BIN,写10进制，硬件自动转换为2进制进行存储
		RTC_SetTime (RTC_Format_BIN, &RTC_TimeStruct1);
		
		
		RTC_DateStruct1.RTC_Year		= 25; 	//年
		RTC_DateStruct1.RTC_Month	= 6;	//月
		RTC_DateStruct1.RTC_Date		= 3;	//日
		RTC_DateStruct1.RTC_WeekDay  = 2;	//星期
		//设置日期：
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct1);
	
		//将BKP写入后备寄存器
		RTC_WriteBackupRegister(RTC_BKP_DR0, BKP);
	}
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
	i2=RTC_TimeStruct.RTC_Hours,i1=RTC_TimeStruct.RTC_Minutes;
	i=RTC_TimeStruct.RTC_Seconds,i3=RTC_DateStruct.RTC_Date;
	if(i==59)
		i1+=1,i=-1;
	if(i1==60)
		i2+=1,i1=0;
	if(i2==24)
		i2=0,i3+=1;
	if(i3==30)
		i3=0;
	RTC_Alarm_AInit(i2,i1,++i,i3);
}
void RTC_Alarm_AInit(uint8_t Hours,uint8_t Minutes,uint8_t Seconds,uint8_t RTC_AlarmDateWeekDay)
{
	RTC_AlarmTypeDef	RTC_AlarmStruct;
	RTC_TimeTypeDef 	RTC_AlarmTime;
	
	//关闭闹钟：
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE); 
	
	//闹钟时间设置
	RTC_AlarmTime.RTC_H12		= RTC_H12_AM; //上午，在24小时制时，这个参数是无用
	RTC_AlarmTime.RTC_Hours		= Hours; //时
	RTC_AlarmTime.RTC_Minutes	= Minutes; //分
	RTC_AlarmTime.RTC_Seconds	= Seconds; //秒

	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime; //时间设置
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;//无掩码位，按实际设置时间来响应
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel	= RTC_AlarmDateWeekDaySel_Date; //按日期响应新课程 
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= RTC_AlarmDateWeekDay;
	
	//配置闹钟参数：
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A, &RTC_AlarmStruct);
	//开启配置闹钟中断：
	
	EXTI->IMR|=EXTI_Line17;
	EXTI->RTSR|=EXTI_Line17;//上降沿
		//NVIC配置
//	SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//中断分组只有10：8可以
	NVIC->ISER[RTC_Alarm_IRQn>>0x5]|=0x1<<(RTC_Alarm_IRQn&0x1F);
	NVIC->IP[RTC_Alarm_IRQn]|=0x6<<4;//高4位有效响中断其中的下标为中断号
	//开启闹钟A中断
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	//开启闹钟：
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
}

void RTC_Alarm_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	u8 i,i1,i2,i3;
	char a[50];
	BaseType_t xReturn;
	//判断中断线状态，如果有中断，中断位标志为1
	if(EXTI_GetITStatus(EXTI_Line17) == SET)
	{
		if(RTC_GetFlagStatus(RTC_FLAG_ALRAF) == SET)
		{
			RTC_GetDate(RTC_Format_BIN,&RTC_DateStruct);
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			
			snprintf(a,sizeof(a),"Date:20%d.%d.%d Week:%d,TIME:%d:%d:%d",RTC_DateStruct.RTC_Year,\
			RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,RTC_DateStruct.RTC_WeekDay,\
			RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
			for(u8 i=0;i<50;i++)
			{
			if(xQueueSendFromISR(g_queue, &a[i], &xHigherPriorityTaskWoken)== pdTRUE)
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			}
			if(xSemaphoreGiveFromISR(g_sem_binary,&xHigherPriorityTaskWoken) == pdTRUE)
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			//清空标志位
			RTC_ClearFlag(RTC_FLAG_ALRAF);
		}
		//清空中断线
		EXTI_ClearITPendingBit(EXTI_Line17);
	}
	i2=RTC_TimeStruct.RTC_Hours,i1=RTC_TimeStruct.RTC_Minutes;
	i=RTC_TimeStruct.RTC_Seconds,i3=RTC_DateStruct.RTC_Date;
	if(i==59)
		i1+=1,i=-1;
	if(i1==60)
		i2+=1,i1=0;
	if(i2==24)
		i2=0,i3+=1;
	if(i3==30)
		i3=0;
	RTC_Alarm_AInit(i2,i1,++i,i3);
	
}
