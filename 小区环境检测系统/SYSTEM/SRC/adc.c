#include "adc.h"



/************************************
����˵����
��������/�ɵ�����������PF7
ADCͨ����ADC_Channel_5����ӦPF7��
PF7��ѹ��Χ: 0~3.3V
************************************/


void Adc_PF7_Init(void)
{
	//�ṹ�����
	GPIO_InitTypeDef		GPIO_InitStruct;
	ADC_CommonInitTypeDef	ADC_CommonInitStruct;
	ADC_InitTypeDef			ADC_InitStruct;
	
	
	//����PF��ʱ�Ӻ�ADC3ʱ�ӣ�����PF7Ϊģ��ģʽ��
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);  
	
	
	GPIO_InitStruct.GPIO_Pin	= GPIO_Pin_7; 	//����5
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AN;	//ģ��ģʽ
	GPIO_InitStruct.GPIO_PuPd	= GPIO_PuPd_NOPULL; //��������
	GPIO_Init(GPIOF, &GPIO_InitStruct);

	
	ADC_CommonInitStruct.ADC_Prescaler	=  ADC_Prescaler_Div4; //84MHZ/4 = 21MHZ  ADCת��Ƶ�ʲ��ܳ���36MHZ
	ADC_CommonInitStruct.ADC_Mode		= ADC_Mode_Independent; //����ģʽ
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //��ʹ��DMA
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_13Cycles; //��������ADC������� ˫�� �������ز�������
	//��ʼ��ADC_CCR�Ĵ�����
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	
	ADC_InitStruct.ADC_Resolution	= ADC_Resolution_12b;//ADC�ֱ���ѡ�� 4096�ȷ�
	ADC_InitStruct.ADC_DataAlign	= ADC_DataAlign_Right; //�����Ҷ���
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//��ʹ��ɨ��
	ADC_InitStruct.ADC_ContinuousConvMode 	= DISABLE; //����ת��
	ADC_InitStruct.ADC_NbrOfConversion = 1; 	//ת��ͨ����Ŀ
	//ʹ���������,�������������
	ADC_InitStruct.ADC_ExternalTrigConv	  	=	ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_ExternalTrigConvEdge	=ADC_ExternalTrigConvEdge_None;  //�ޱ��ؼ��
	ADC_Init(ADC3, &ADC_InitStruct);
	//ʹ��ADC��
	 ADC_Cmd(ADC3, ENABLE);
	//���ù���ͨ��������
	//��������:1 ��һ��ת��
	ADC_RegularChannelConfig(ADC3, ADC_Channel_5, 1, ADC_SampleTime_15Cycles);
	

}
void Adc_PA3_Init(void)
{
	//�ṹ�����
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); // ʹ��ʱ��

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE); // ʹ��ADCʱ��

	GPIO_InitTypeDef  GPIO_InitStructure; // GPIO���ýṹ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;    //ģ������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;    //�������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIO
			 
	/* ADC�������� */
	ADC_CommonInitTypeDef ADC_CommonInitStruct; // ADC�������ýṹ��

//	ADC_DeInit();//ADC��λ

	ADC_CommonInitStruct.ADC_DMAAccessMode=ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode=ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler=ADC_Prescaler_Div4;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay=ADC_TwoSamplingDelay_5Cycles;

	ADC_CommonInit(&ADC_CommonInitStruct);
	
	/* ADC�ض����� */
	ADC_InitTypeDef ADC_InitStruct; // ADC�ض����ýṹ��
	
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion=1;
	ADC_InitStruct.ADC_Resolution=ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1, &ADC_InitStruct);
	
	//���òɼ�ͨ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_480Cycles );
	
	//ADCʹ��
	ADC_Cmd(ADC1, ENABLE); 
}
u16 Get_PF7_Adc_Value(void)
{
	u16 value;
	
	//�������ת����
	
	ADC_SoftwareStartConv(ADC3);
	
	//�ȴ�ת����־���
	while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);

	value =  ADC_GetConversionValue(ADC3);
	
	return value;
}
u16 Get_PA3_Adc_Value(void)
{
	uint32_t Data = 0;
	
	//��ʼ���ת��
	ADC_SoftwareStartConv(ADC1);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )); // �ȴ�ת������
	
	for(int i = 0; i < 30; i++)
	{
		Data += ADC_GetConversionValue(ADC1);
		
		delay_ms(2);
	}
	
	Data = Data / 30;
	
	return Data;
}

