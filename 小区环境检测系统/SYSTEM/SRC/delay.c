#include "includes.h"                  // Device header

u32 my_nus = 21;  		//��21MHZ����21��������ʱ1us
u32 my_nms = 21000;  	//��21MHZ����21000��������ʱ1ms

void Delay_Init(void)
{
	//HCLK = 168MHZ  HCLK/8 = 21MHZ
	//SysTick��ʱ����ʱ��Ƶ��:21MHZ
	//ʹ���ⲿ�ο���ʱ��
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

}

//nus:ȡֵ��Χ��1~798,915
void delay_us(int nus)
{
	uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
	
	//���ֵ�̶�SysTick->LOAD == 999
    uint32_t reload=SysTick->LOAD;    //ϵͳ��ʱ��������ֵ   
	//SystemCoreClock = 168000 000
	//ticks = 168*nus
    ticks=nus*(SystemCoreClock/1000000);//��Ҫ�Ľ����� 
	//���������������仯��
    told=SysTick->VAL;            //�ս���ʱ�ļ�����ֵ
 
    /* ���������[��ѡ,�ᵼ�¸����ȼ������޷���ռ��ǰ���񣬵��ܹ���ߵ�ǰ����ʱ��ľ�ȷ��] */
    vTaskSuspendAll();    
 
    while(1)
    {
        tnow=SysTick->VAL;
        
        if(tnow!=told)
        {     
            /* SYSTICK��һ���ݼ��ļ����� */
            if(tnow<told)
                tcnt+=told-tnow;        
            else 
                tcnt+=reload-tnow+told+1;    
            
            told=tnow;
            
            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��ticks,���˳���*/
            if(tcnt>=ticks)
                break;            
        }  
    }

    /* �ָ�������[��ѡ] */
    xTaskResumeAll();
}



//nms:ȡֵ��Χ��1~798
void delay_ms(int nms)
{
	vTaskDelay(nms);
}

void delay_s(int ns)
{
	int i;
	for(i=0; i<ns; i++)
	{
		vTaskDelay(1000);
	}
}

