#include "adc.h"



/************************************
引脚说明：
光敏电阻/可调电阻连接在PF7
ADC通道：ADC_Channel_5（对应PF7）
PF7电压范围: 0~3.3V
************************************/


void Adc_PF7_Init(void)
{
	//结构体变量
	GPIO_InitTypeDef		GPIO_InitStruct;
	ADC_CommonInitTypeDef	ADC_CommonInitStruct;
	ADC_InitTypeDef			ADC_InitStruct;
	
	
	//开启PF口时钟和ADC3时钟，设置PF7为模拟模式。
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);  
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_7; 	//引脚5
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AN;	//模拟模式
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL; //无上下拉
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	
	ADC_CommonInitStruct.ADC_Prescaler	=  ADC_Prescaler_Div4; //84MHZ/4 = 21MHZ  ADC转换频率不能超过36MHZ
	ADC_CommonInitStruct.ADC_Mode		= ADC_Mode_Independent; //独立模式
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //不使用DMA
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_13Cycles; //两个输入ADC采样间隔 双重 或者三重才起作用
	//初始化ADC_CCR寄存器。
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	
	ADC_InitStruct.ADC_Resolution	= ADC_Resolution_12b;//ADC分辨率选择 4096等分
	ADC_InitStruct.ADC_DataAlign	= ADC_DataAlign_Right; //数据右对齐
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不使用扫描
	ADC_InitStruct.ADC_ContinuousConvMode 	= DISABLE; //单次转换
	ADC_InitStruct.ADC_NbrOfConversion = 1; 	//转换通道数目
	//使用软件触发,下面参数无意义
	ADC_InitStruct.ADC_ExternalTrigConv	  	=	ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_ExternalTrigConvEdge	=ADC_ExternalTrigConvEdge_None;  //无边沿检测
	ADC_Init(ADC3, &ADC_InitStruct);
	//使能ADC。
	 ADC_Cmd(ADC3, ENABLE);
	//配置规则通道参数：
	//规则序列:1 第一个转换
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_15Cycles);
	

}
void Adc_PA3_Init(void)
{
	//结构体变量
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); // 使能时钟

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE); // 使能ADC时钟

	GPIO_InitTypeDef  GPIO_InitStructure; // GPIO配置结构体
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;    //模拟输入
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //推挽输出
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO
			 
	/* ADC公共配置 */
	ADC_CommonInitTypeDef ADC_CommonInitStruct; // ADC公共配置结构体

//	ADC_DeInit();//ADC复位

	ADC_CommonInitStruct.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode=ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;

	ADC_CommonInit(&ADC_CommonInitStruct);
	
	/* ADC特定配置 */
	ADC_InitTypeDef ADC_InitStruct; // ADC特定配置结构体
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion=1;
	ADC_InitStruct.ADC_Resolution=ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1, &ADC_InitStruct);
	
	//设置采集通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_480Cycles );
	
	//ADC使能
	ADC_Cmd(ADC1, ENABLE); 
}
u16 Get_PF7_Adc_Value(void)
{
	u16 value;
	
	//开启软件转换：
	
	ADC_SoftwareStartConv(ADC3);
	
	//等待转换标志完成
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);

	value =  ADC_GetConversionValue(ADC3);
	
	return value;
}
u16 Get_PA3_Adc_Value(void)
{
	uint32_t Data = 0;
	
	//开始软件转换
	ADC_SoftwareStartConv(ADC1);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )); // 等待转换结束
	
	for(int i = 0; i < 30; i++)
	{
		Data += ADC_GetConversionValue(ADC1);
		
		delay_ms(2);
	}
	
	Data = Data / 30;
	
	return Data;
}

