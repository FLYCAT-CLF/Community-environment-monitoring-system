#include "dht11.h"


/**********************************
����˵����

PG9 -- DQ(DHT11)
***********************************/

void Dht11_Init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;				//��9������
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;			//���ģʽ
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_25MHz;			//���ŵ��ٶ����Ϊ25MHz
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;			//û��ʹ���ڲ���������
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	//��ʪ��ģ�黹û�й�������ô���Ĵ��������Ǹߵ�ƽ
	PGout(9) = 1;
}

//����ģʽ���
void Dht11_Pin_Mode(GPIOMode_TypeDef mode)
{
	GPIO_InitTypeDef  	GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;				//��9������
	GPIO_InitStructure.GPIO_Mode  = mode;					//����/���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�����������ǿ�������������ŵ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;		//���ŵ��ٶ����Ϊ100MHz
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;		//û��ʹ���ڲ���������
	GPIO_Init(GPIOG, &GPIO_InitStructure);	
}

//����DHT11
int Dht11_Start(void)
{
	u16 t = 0;
	
	
	//�����ź� 
	//��Ϊ���ģʽ
	Dht11_Pin_Mode(GPIO_Mode_OUT);
	PGout(9) = 1;
	delay_ms(2);
	PGout(9) = 0;
	delay_ms(20);
	PGout(9) = 1;
	delay_us(30);
	
	//�����ź� 
	//��Ϊ����ģʽ
	Dht11_Pin_Mode(GPIO_Mode_IN);	
	//�жϻ����ź��е���ߵ�ƽ����
	
	
	//�����ǱȽϼ򵥵Ĺ��˻����źŵĴ��룬������룬���Ӳ���л����п��Գ��ֿ�������
	//while(PGin(9) == 1);
	//while(PGin(9) == 0);
	//while(PGin(9) == 1);
	
	
	
	//�ȴ��͵�ƽ�ĵ���
	while(PGin(9) == 1)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //�ȴ�200usδ�е͵�ƽ�������򷵻�
			return -1;
	}
	//��ʱ40us���ٹ��˵͵�ƽ
	delay_us(40);
	
	t = 0;
	//�ȴ��ߵ�ƽ�ĵ���,���˵͵�ƽ
	while(PGin(9) == 0)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //�ȴ�200usδ�иߵ�ƽ�������򷵻�
			return -1;
	}	
	
	//��ʱ40us
	delay_us(40);	
	
	t = 0;
	//�ȴ��͵�ƽ�ĵ��������˸ߵ�ƽ
	while(PGin(9) == 1)
	{
		t++;
		delay_us(2);
		
		if(t >= 100) //�ȴ�200usδ�е͵�ƽ�������򷵻�
			return -1;
	}	
	
	//ִ�е��������������DHT11����40λ����
	
	return 0;
	

}

//����һ���ֽ�
u8 Dht11_Recv_Byte(u8 *s)
{
	u16 t = 0;
	for(int a=0;a<5;a++)
	for(u8 i=0; i<8; i++)
	{
		t = 0;
		//�ȴ��ߵ�ƽ�ĵ���,���˵͵�ƽ
		while(PGin(9) == 0)
		{
			t++;
			delay_us(2);
			
			if(t >= 100) //�ȴ�200usδ�иߵ�ƽ�������򷵻�
				return 0;
		}	
			
		//�����������˵���Ǹߵ�ƽ
		delay_us(40);
//		TIM3->CR1 |= TIM_CR1_CEN;
		if(PGin(9))
		{
			//�ȷ���λ���Ⱥϳɸ�λ
			*(s+a) |= (1<<(7-i));
			
			//���д��30us�ߵ�ƽ
			t = 0;
			//�ȴ��͵�ƽ�ĵ��������˸ߵ�ƽ
			while(PGin(9) == 1)
			{
				t++;
				delay_us(2);
				if(t >= 100) //�ȴ�200usδ�е͵�ƽ�������򷵻�
					return 0;
			}			
		}
	}
	return 1;
	
}




