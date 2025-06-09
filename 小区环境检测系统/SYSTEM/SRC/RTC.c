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
		//����RTCʱ��Դ��ʹ��RTCʱ�ӣ�
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//���ʹ��LSE��Ҫ��LSE��
		RCC_LSEConfig(RCC_LSE_ON);
		
		//�ȴ�ʱ��Դ�ȶ�
		//����1
		//delay_ms(20);
		/* Wait till LSE is ready */  
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{
			
		}
		RTC_InitStruct.RTC_HourFormat	= RTC_HourFormat_24; //Сʱ��ѡ��
		RTC_InitStruct.RTC_AsynchPrediv	= 0x7F;		//�첽Ԥ��Ƶ�� 128��Ƶ
		RTC_InitStruct.RTC_SynchPrediv	= 0xFF;		//ͬ��Ԥ��Ƶ�� 256��Ƶ
		//��ʼ��RTC(ͬ��/�첽��Ƶϵ����ʱ�Ӹ�ʽ)��
		RTC_Init(&RTC_InitStruct);
		
		RTC_TimeStruct1.RTC_H12		= RTC_H12_PM; //���磬��24Сʱ��ʱ���������������
		RTC_TimeStruct1.RTC_Hours	= 12; //ʱ
		RTC_TimeStruct1.RTC_Minutes	= 0; //��
		RTC_TimeStruct1.RTC_Seconds	= 0; //��
		//RTC_Format_BIN,д10���ƣ�Ӳ���Զ�ת��Ϊ2���ƽ��д洢
		RTC_SetTime (RTC_Format_BIN, &RTC_TimeStruct1);
		
		
		RTC_DateStruct1.RTC_Year		= 25; 	//��
		RTC_DateStruct1.RTC_Month	= 6;	//��
		RTC_DateStruct1.RTC_Date		= 3;	//��
		RTC_DateStruct1.RTC_WeekDay  = 2;	//����
		//�������ڣ�
		RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct1);
	
		//��BKPд��󱸼Ĵ���
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
	
	//�ر����ӣ�
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE); 
	
	//����ʱ������
	RTC_AlarmTime.RTC_H12		= RTC_H12_AM; //���磬��24Сʱ��ʱ���������������
	RTC_AlarmTime.RTC_Hours		= Hours; //ʱ
	RTC_AlarmTime.RTC_Minutes	= Minutes; //��
	RTC_AlarmTime.RTC_Seconds	= Seconds; //��

	
	RTC_AlarmStruct.RTC_AlarmTime			= RTC_AlarmTime; //ʱ������
	RTC_AlarmStruct.RTC_AlarmMask			= RTC_AlarmMask_None;//������λ����ʵ������ʱ������Ӧ
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel	= RTC_AlarmDateWeekDaySel_Date; //��������Ӧ�¿γ� 
	RTC_AlarmStruct.RTC_AlarmDateWeekDay	= RTC_AlarmDateWeekDay;
	
	//�������Ӳ�����
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A, &RTC_AlarmStruct);
	//�������������жϣ�
	
	EXTI->IMR|=EXTI_Line17;
	EXTI->RTSR|=EXTI_Line17;//�Ͻ���
		//NVIC����
//	SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//�жϷ���ֻ��10��8����
	NVIC->ISER[RTC_Alarm_IRQn>>0x5]|=0x1<<(RTC_Alarm_IRQn&0x1F);
	NVIC->IP[RTC_Alarm_IRQn]|=0x6<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�
	//��������A�ж�
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	//�������ӣ�
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
}

void RTC_Alarm_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	u8 i,i1,i2,i3;
	char a[50];
	BaseType_t xReturn;
	//�ж��ж���״̬��������жϣ��ж�λ��־Ϊ1
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
			//��ձ�־λ
			RTC_ClearFlag(RTC_FLAG_ALRAF);
		}
		//����ж���
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
