#ifndef __RTC_H_
#define __RTC_H_

#include "stm32f4xx.h"                  // Device header
extern RTC_DateTypeDef RTC_DateStruct;
extern RTC_TimeTypeDef RTC_TimeStruct;
void rtc_init(void);
void RTC_Alarm_AInit(uint8_t Hours,uint8_t Minutes,uint8_t Seconds,uint8_t RTC_AlarmDateWeekDay);
#endif
