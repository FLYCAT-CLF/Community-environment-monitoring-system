#ifndef __ADC_H
#define __ADC_H

#include "stm32f4xx.h"
#include "sys.h"
int Filter(int m);
void Adc_PF7_Init(void);
u16 Get_PF7_Adc_Value(void);
void Adc_PA3_Init(void);
u16 Get_PA3_Adc_Value(void);
#endif
