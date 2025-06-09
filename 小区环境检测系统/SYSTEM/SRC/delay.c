#include "includes.h"                  // Device header

u32 my_nus = 21;  		//在21MHZ，计21个数，用时1us
u32 my_nms = 21000;  	//在21MHZ，计21000个数，用时1ms

void Delay_Init(void)
{
	//HCLK = 168MHZ  HCLK/8 = 21MHZ
	//SysTick定时器的时钟频率:21MHZ
	//使用外部参考的时钟
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

}

//nus:取值范围：1~798,915
void delay_us(int nus)
{
	uint32_t ticks;
    uint32_t told,tnow,tcnt=0;
	
	//这个值固定SysTick->LOAD == 999
    uint32_t reload=SysTick->LOAD;    //系统定时器的重载值   
	//SystemCoreClock = 168000 000
	//ticks = 168*nus
    ticks=nus*(SystemCoreClock/1000000);//需要的节拍数 
	//这个计数器，这个变化的
    told=SysTick->VAL;            //刚进入时的计数器值
 
    /* 挂起调度器[可选,会导致高优先级任务无法抢占当前任务，但能够提高当前任务时间的精确性] */
    vTaskSuspendAll();    
 
    while(1)
    {
        tnow=SysTick->VAL;
        
        if(tnow!=told)
        {     
            /* SYSTICK是一个递减的计数器 */
            if(tnow<told)
                tcnt+=told-tnow;        
            else 
                tcnt+=reload-tnow+told+1;    
            
            told=tnow;
            
            /* 时间超过/等于要延迟的时间ticks,则退出。*/
            if(tcnt>=ticks)
                break;            
        }  
    }

    /* 恢复调度器[可选] */
    xTaskResumeAll();
}



//nms:取值范围：1~798
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

