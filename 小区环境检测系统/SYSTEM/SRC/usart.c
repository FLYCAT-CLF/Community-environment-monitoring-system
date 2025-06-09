#include "includes.h"                 
volatile uint8_t  g_usart1_rx_buf[1024];
volatile uint32_t g_usart1_rx_cnt=0;
volatile uint32_t g_usart1_rx_end=0;
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
    int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
    x = x;
return 0;
}


void usart1_init(u32 myBaudRate)
{
	USART_InitTypeDef USART_InitStruct1;

	RCC->AHB1ENR|=0x1;
	RCC->APB2ENR|=RCC_APB2Periph_USART1;
	GPIOA->MODER|=0xA<<(2*9);
	GPIOA->PUPDR|=0x5<<9;
	GPIOA->AFR[1]|=GPIO_AF_USART1<<4|GPIO_AF_USART1<<8;
	
	USART_InitStruct1.USART_BaudRate=myBaudRate;
	USART_InitStruct1.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct1.USART_Parity=USART_Parity_No;
	USART_InitStruct1.USART_StopBits=USART_StopBits_1;
	USART_InitStruct1.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct1.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	
	USART_Init(USART1,&USART_InitStruct1);
	
//	SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//�жϷ���ֻ��10��8����
	NVIC->ISER[USART1_IRQn>>0x05]|=0x1<<(USART1_IRQn &0x1F);//�����ж�0-31�����е�ֵλ�жϺ�(NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
	NVIC->IP[USART1_IRQn]|=0x1<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�
	//����Ϊ�����жϣ���ʾ�����ݹ�����CPUҪ�жϽ��н��գ�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

	//ʹ�ܴ��ڡ�
	USART1->CR1 |= USART_CR1_UE;
	
}
void usart3_init(u32 myBaudRate)
{
	USART_InitTypeDef USART_InitStruct3;

	RCC->AHB1ENR|=0x1<<1;
	RCC->APB1ENR|=RCC_APB1Periph_USART3;
	GPIOB->MODER|=0xA<<(2*10);
	GPIOB->PUPDR|=0x5<<10;
	GPIOB->AFR[1]|=GPIO_AF_USART3<<8|GPIO_AF_USART3<<12;
	
	USART_InitStruct3.USART_BaudRate=myBaudRate;
	USART_InitStruct3.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct3.USART_Parity=USART_Parity_No;
	USART_InitStruct3.USART_StopBits=USART_StopBits_1;
	USART_InitStruct3.USART_WordLength=USART_WordLength_8b;
	USART_InitStruct3.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	
	USART_Init(USART3,&USART_InitStruct3);
	
//	SCB->AIRCR|=(0x05FA << 16)|(0x2<<8);//�жϷ���ֻ��10��8����
	NVIC->ISER[USART3_IRQn>>0x05]|=0x1<<(USART3_IRQn &0x1F);//�����ж�0-31�����е�ֵλ�жϺ�(NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
	NVIC->IP[USART3_IRQn]|=0x1<<4;//��4λ��Ч���ж����е��±�Ϊ�жϺ�
	//����Ϊ�����жϣ���ʾ�����ݹ�����CPUҪ�жϽ��н��գ�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 

	//ʹ�ܴ��ڡ�
	USART3->CR1 |= USART_CR1_UE;
	
}
void usart3_send_str(const char *pbuf)
{
    const char *p = pbuf;
    
       //��⵱ǰp��ָ����Ч��
     //*p����Ƿ�Ϊ��0  
     while(p && *p)
     {
        USART3->DR=*p++;
        while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
				USART_ClearFlag(USART3,USART_FLAG_TXE);
     }
}
void usart1_send_bytes(uint8_t *buf,uint32_t len)
{
	uint8_t *p = buf;
	
	while(len--)
	{
		USART1->DR=*p;
		
		p++;
		
		//�ȴ����ݷ��ͳɹ�
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
		USART_ClearFlag(USART1,USART_FLAG_TXE);
	}
}

int fputc(int ch,FILE *f)
{
	USART1->DR=ch;
//	 while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	while((USART1->SR&=0x1<<7)==0);
	return ch;
}
u8 g_buffer[32] = {0}, g_rxbuffer[32]={0};
u8 g_rxflag = 0;
u8 g_count = 0, g_i = 0;
//ÿ����һ���ֽڶ�������ж�
void USART1_IRQHandler(void)
{
	
	//�жϴ��ڽ��ձ�־λ�ı�־λ
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		//��ձ�־λ���������һλ����
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//��������
		g_buffer[g_count++] = USART_ReceiveData(USART1);
		
		//����յ�����Ϊ':'��ʾ�յ�������
		if(g_buffer[g_count-1] == ':')
		{
			//��g_buffer���¸�ֵ��g_rxbuffer���ҹ��˽�����
			for(g_i=0; g_i < g_count-1; g_i++)
			{
				g_rxbuffer[g_i] = g_buffer[g_i];
			}
			
			g_rxflag = 1; //��ʾ���ݽ������
			
			g_count = 0; //��һ���ݴ�g_buffer[0]��ʼ�洢
			
			memset(g_buffer, 0, sizeof(g_buffer));
			
		}
	
		printf("%c", g_buffer[g_count-1]);
		
		//printf("%c\r\n", g_buffer[g_count-1]);
		
	}	
}
void USART3_IRQHandler(void)
{
	uint8_t d=0;
	
	//����Ƿ���յ�����
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		d=USART_ReceiveData(USART3);
		
		
		g_esp8266_rx_buf[g_esp8266_rx_cnt++]=d;
		
		if(g_esp8266_rx_cnt >= sizeof g_esp8266_rx_buf)
		{
			g_esp8266_rx_end=1;
		}

	
		//�����յ������ݷ�����PC
//		USART_SendData(USART1,d);
//		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	
		//��ձ�־λ��������Ӧ�µ��ж�����
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}
void TIM3_IRQHandler(void)
{
	static uint32_t usart1_rx_cnt_last=0;	
	static uint32_t usart3_rx_cnt_last=0;
	
	//���ʱ������жϵı�־λ�Ƿ���λ
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		if(g_esp8266_transparent_transmission_sta && g_esp8266_rx_cnt)
		{
			//����ȣ����ʾ����3Ŀǰ�����������
			if(g_esp8266_rx_cnt == usart3_rx_cnt_last)
			{
				g_esp8266_rx_end=1;
			}
			else
			{
				usart3_rx_cnt_last = g_esp8266_rx_cnt;
			}
		
		}
		
		if(g_usart1_rx_cnt)
		{
			//����ȣ����ʾ����1Ŀǰ�����������
			if(g_usart1_rx_cnt == usart1_rx_cnt_last)
			{
				g_usart1_rx_end=1;
			}
			else
			{
				usart1_rx_cnt_last = g_usart1_rx_cnt;
			}
		
		}		
		//��ձ�־λ
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
