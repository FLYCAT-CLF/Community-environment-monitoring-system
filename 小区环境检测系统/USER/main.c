#include "includes.h"
static TaskHandle_t init_task_handle = NULL;
static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;
static TaskHandle_t task3_handle = NULL;
static TaskHandle_t task4_handle = NULL;
static SemaphoreHandle_t xMutex = NULL;
QueueHandle_t  g_queue;
SemaphoreHandle_t  g_sem_binary;
static SemaphoreHandle_t  g_sem_binary1;
static SemaphoreHandle_t  g_sem_binary2;
char *p = "GET https://v.api.aa1.cn/api/time-tx/index.php \r\n";
char *p1 = "GET /v3/weather/now.json?key=Sv5Y46v3KE0dOj-I_&location=guangzhou&language=zh-Hans&unit=c HTTP/1.1\r\nHost: api.seniverse.com\r\n\r\n";
u8 ht[5],si;
int zeller_weekday(int year, int month, int day) {
    // 关键修正：只有1月和2月需要调整为上一年的13月和14月
    if (month < 3) {
        month += 12;
        year--;
    }
    int c = year / 100;      // 世纪数（前两位）
    int y = year % 100;      // 年份（后两位）
    int w = (day + 13 * (month + 1) / 5 + y + y / 4 + c / 4 - 2 * c) % 7;
    return (w + 7) % 7;      // 确保结果为非负数（0-6）
}

static void init_task(void* pvParameters)
{
		xMutex = xSemaphoreCreateMutex();
		SCB->AIRCR|=(0x05FA << 16)|(0x3<<8);	
		g_sem_binary = xSemaphoreCreateBinary();
		g_sem_binary1 = xSemaphoreCreateBinary();
		g_sem_binary2 = xSemaphoreCreateBinary();
		g_queue = xQueueCreate(50, 8);
    if (g_sem_binary == NULL) 
		{
        // 信号量创建失败，进行错误处理
        while(1);
    }
		
		usart1_init(115200);
		usart3_init(115200);
		Dht11_Init();     
		MP3_init();
		IIC_init();
		Adc_PA3_Init();
		Adc_PF7_Init();
		
		rtc_init();
		tim3_init();
		g_esp8266_rx_cnt = 0;
		memset(g_esp8266_rx_buf, 0, sizeof g_esp8266_rx_buf);
		g_esp8266_rx_end = 0;

		if(xSemaphoreTake(xMutex,portMAX_DELAY) == pdTRUE)
		{
				OLED_init();
			OLED_Fill(0x00);
			xSemaphoreGive(xMutex);
		}
			
//OLED显示初始化时候不能有其他的OLED函数在其延时的时候执行可以通过互斥锁
		vTaskDelete(NULL);
		
}
//需要将采集到的数据进行显示，对每秒的时间要求并不是特别高。
static void task1(void* pvParameters)
{
		int u,pm,ret,pm1;
		float Voltage;
		char *s="brightness:";
		char s1[10],temp[25],hum[25],pmk[25];
		while(1)
		{
			si=((4095-Get_PF7_Adc_Value())/4095.0)*100;
			sprintf(s1,"%d%%",si);
			if(xSemaphoreTake(xMutex,portMAX_DELAY) == pdTRUE)
			{
				GPIOA->ODR|=0x1<<2;
				delay_us(280);
				Voltage = 3.3f * Get_PA3_Adc_Value() / 4096.f ; //获得AO输出口的电压值
				delay_us(38);
				GPIOA->ODR&=~(0x1<<2);
				delay_us(9680);
				pm = (0.17*Voltage-0.1)*1000;  //乘以1000单位换成ug/m3/
				if (pm < 0)
					pm = 0;            
				if (pm>500)        
					pm=500;
				sprintf(pmk,"pm2.5:%dug/m3",pm);
				label:
				ret = Dht11_Start();
				if(ret == 0) 
				{
					memset(ht,0,sizeof(ht));
					if(Dht11_Recv_Byte(ht)==0)
						printf("DHT11_Recv Failed...");
					if(*ht+*(ht+1)+*(ht+2)+*(ht+3)!=*(ht+4))
					{
						printf("DHT11-error");
						goto label;
					}
					sprintf(hum,"humidity:%d.%d%%RH",*ht,*(ht+1));
					if(*(ht+3)>>7)
					{
						if(*(ht+3)/10)
							sprintf(temp,"temperature:-%d.%dC",*(ht+2),*(ht+3));
						else
							sprintf(temp,"temperature:-%d.0%dC",*(ht+2),*(ht+3));
					}
					else
					{
						if(*(ht+3)/10)
							sprintf(temp,"temperature:%d.%dC",*(ht+2),*(ht+3));
						else
							sprintf(temp,"temperature:%d.0%dC",*(ht+2),*(ht+3));
					}
					OLED_ShowStr(0,11,hum,1);
					OLED_ShowStr(0,12,temp,1);
				}
				OLED_ShowStr(0,10,s,1);
				if(u/10==0)
					OLED_ShowStr(strlen(s)*6,10,"    ",1);
				OLED_ShowStr(strlen(s)*6,10,s1,1);
				
				if(pm!=pm1&&(pm/10==0||pm/100==0))
				{
					OLED_ShowStr(0,13,"         ",1);
				}
				pm1=pm;
				OLED_ShowStr(0,13,pmk,1);
				xSemaphoreGive(xMutex);
			}
		delay_ms(100);
		}

} 
//需要rtc每秒相应的任务功能例如：实时时间
static void task2(void* pvParameters)
{
		char date[30]={0};
		char time[20]={0};
		char newstime[20]={0};
		static char o[50];
		static u8 t=240,t1=0;
		char *c,*j,*tf,a[50];
		while(1)
		{
				if(xSemaphoreTake(g_sem_binary,portMAX_DELAY) == pdTRUE)
				{
					if(t1==60)
					{
						t++;
						t1=0;
					}
					if(t==240)
					{
						xSemaphoreGive(g_sem_binary1);
						t=0;
					}
					t1++;
					if(xSemaphoreTake(xMutex,portMAX_DELAY) == pdTRUE)
					{
						int itemsToReceive = uxQueueMessagesWaiting(g_queue);
						for (u8 i = 0; i < itemsToReceive; i++) 
							xQueueReceive(g_queue, &a[i], 0);
						if(strlen(a)!=strlen(o))
						{
							OLED_ShowStr(5,0,"             ",1);
							OLED_ShowStr(5,9,"             ",1);
						}
					strcpy(o,a);
					c=strstr(a,",");
					j=strstr(c,",");
					j+=1;
					*c='\0';
					strncpy(date,a,strlen(a));
					OLED_ShowStr(0,0,date,1);
					OLED_ShowStr(0,9,j,1);
					xSemaphoreGive(xMutex);
					}
				}
				delay_ms(100);
		}
}   
static void task3(void* pvParameters)
{
		char newstime[20]={0};
		static char o[50];
		char *c,*j,*tf;
		u8 time;
		u8 a,a1,a2,a3,tb=1;
		while(1)
		{
		if(xSemaphoreTake(g_sem_binary1,portMAX_DELAY) == pdTRUE)
		{
			while(esp8266_init())
			printf("esp8266_mqtt_init ...");
			while(tb)
			{
			esp8266_send_bytes(p);
			delay_ms(500);
			printf("g_esp8266_rx_cnt:%d\r\n", g_esp8266_rx_cnt);
			if(g_esp8266_rx_end && g_esp8266_transparent_transmission_sta && g_esp8266_rx_cnt>50)
			{
				tb=0;
				xSemaphoreGive(g_sem_binary2);
				for(int i=0;i<g_esp8266_rx_cnt+1;i++)
				{	
					//解析时间cjson
					if(g_esp8266_rx_buf[i] == '{')
					{
						mqtt_cjson_parse((char*)&g_esp8266_rx_buf[i],newstime);
						break;
					}
				}
				time= ((*(newstime+2)-'0')*10)+(*(newstime+3)-'0');
				RTC_DateStruct.RTC_Year		=time; 	//年
				time= ((*(newstime+5)-'0')*10)+(*(newstime+6)-'0');
				RTC_DateStruct.RTC_Month	= time;	//月
				time= ((*(newstime+8)-'0')*10)+(*(newstime+9)-'0');
				RTC_DateStruct.RTC_Date		= time;	//日
				if(zeller_weekday(2000+((*(newstime+2)-'0')*10)+(*(newstime+3)-'0'),((*(newstime+5)-'0')*10)+(*(newstime+6)-'0'),((*(newstime+8)-'0')*10)+(*(newstime+9)-'0'))==0)
					RTC_DateStruct.RTC_WeekDay  = 6;
				else if(zeller_weekday(2000+((*(newstime+2)-'0')*10)+(*(newstime+3)-'0'),((*(newstime+5)-'0')*10)+(*(newstime+6)-'0'),((*(newstime+8)-'0')*10)+(*(newstime+9)-'0'))==1)
					RTC_DateStruct.RTC_WeekDay  = 7;
				else
					RTC_DateStruct.RTC_WeekDay  = zeller_weekday(2000+((*(newstime+2)-'0')*10)+(*(newstime+3)-'0'),((*(newstime+5)-'0')*10)+(*(newstime+6)-'0'),((*(newstime+8)-'0')*10)+(*(newstime+9)-'0'))-1;
				RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
			
				tf=strstr(newstime,"M");
				tf=strstr(tf," ");
	//			printf("s=%s",tf);
				time=((*(tf+1)-'0')*10)+(*(tf+2)-'0');
				RTC_TimeStruct.RTC_Hours	= time; //时
				tf=strstr(tf,":");
				time=((*(tf+1)-'0')*10)+(*(tf+2)-'0');
				RTC_TimeStruct.RTC_Minutes	= time; //分
				tf=strstr(tf+1,":");
				time=((*(tf+1)-'0')*10)+(*(tf+2)-'0'); 
				RTC_TimeStruct.RTC_Seconds	=time; //秒
		
				//RTC_Format_BIN,写10进制，硬件自动转换为2进制进行存储
				RTC_SetTime (RTC_Format_BIN, &RTC_TimeStruct);
				
				RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
				
				a2=RTC_TimeStruct.RTC_Hours,a1=RTC_TimeStruct.RTC_Minutes;
				a=RTC_TimeStruct.RTC_Seconds,a3=RTC_DateStruct.RTC_Date;
				if(a==59)
					a1+=1,a=-1;
				if(a1==60)
					a2+=1,a1=0;
				if(a2==24)
					a2=0,a3+=1;
				if(a3==30)
					a3=0;
				RTC_Alarm_AInit(a2,a1,++a,a3);
				
				g_esp8266_rx_cnt=0;
				g_esp8266_rx_end=0;
			}
			}
		}
		delay_ms(100);
		}
} 
static void task4(void* pvParameters)
{
		u8 check=0,tb=1;
		char weather[10]={0};
		char *y[]={"小雨","中雨","大雨","暴雨","大暴雨","特大暴雨","冻雨","阵雨"};
		char *x[]={"小雪","中雪","大雪","暴雪","雨夹雪","阵雪"};
		while(1)
		{
			if(xSemaphoreTake(g_sem_binary2,portMAX_DELAY) == pdTRUE)
			{
				while(esp8266_init1())
					printf("esp8266_mqtt_init ...");
				while(tb)
				{
					esp8266_send_bytes(p1);
					delay_ms(500);
					printf("g_esp8266_rx_cnt:%d\r\n", g_esp8266_rx_cnt);
					if(g_esp8266_rx_end && g_esp8266_transparent_transmission_sta && g_esp8266_rx_cnt>50)
					{
					for(int i=0;i<g_esp8266_rx_cnt+1;i++)
					{	
						//解析时间cjson
						if(g_esp8266_rx_buf[i] == '{')
						{
							mqtt_cjson_parse1((char*)&g_esp8266_rx_buf[i],weather);
							break;
						}
					}
					tb=0;
					RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
					lx_figure(RTC_DateStruct.RTC_Year/10);
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_figure(RTC_DateStruct.RTC_Year%10);
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_char("年");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(RTC_DateStruct.RTC_Month/10)
					{
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(RTC_DateStruct.RTC_Month%10)
					{
						lx_figure(RTC_DateStruct.RTC_Month%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("月");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(RTC_DateStruct.RTC_Date/10)
					{
						lx_figure(RTC_DateStruct.RTC_Date/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(RTC_DateStruct.RTC_Date%10)
					{
						lx_figure(RTC_DateStruct.RTC_Date%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("日");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(RTC_TimeStruct.RTC_Hours/10)
					{
						lx_figure(RTC_TimeStruct.RTC_Hours/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(RTC_TimeStruct.RTC_Hours%10)
					{
						lx_figure(RTC_TimeStruct.RTC_Hours%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("点");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(RTC_TimeStruct.RTC_Minutes/10)
					{
						lx_figure(RTC_TimeStruct.RTC_Minutes/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(RTC_TimeStruct.RTC_Minutes%10)
					{
						lx_figure(RTC_TimeStruct.RTC_Minutes%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("分");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_char("小区内温度");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(*(ht+2)/10)
					{
						lx_figure(*(ht+2)/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_figure(*(ht+2)%10);
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_char("点");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(*(ht+3)/10!=0||*(ht+3)%10!=0)
					{
						lx_figure(*(ht+3)/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(*(ht+3)%10)
					{
						lx_figure(*(ht+3)%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("摄氏度");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_char("相对湿度百分之");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(*ht/10)
					{
						lx_figure(*ht/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					if(*ht%10)
					{
						lx_figure(*ht%10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_char("光照强度百分之");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					if(si/10)
					{
						lx_figure(si/10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
						lx_figure(10);
						while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					}
					lx_figure(si%10);
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					lx_char("天气");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					for(u8 i=0;i<8;i++)
					{
						if(strcmp(weather,y[i])==0)
							strcpy(weather,"雨");
					}
					for(u8 i=0;i<6;i++)
					{
						if(strcmp(weather,x[i])==0)
							strcpy(weather,"雪");
					}
					check=lx_char("晴");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					check?lx_char("适宜出门"):lx_char("不宜出门");
					while((GPIOD->IDR&(0x1<<8))==0)delay_ms(30);
					g_esp8266_rx_cnt=0;
					g_esp8266_rx_end=0;
				}
			}
		}
		delay_ms(100);
		}
}
int main()
{
	xTaskCreate((TaskFunction_t )init_task,  		/* 任务入口函数 */
			  (const char*    )"init_task",			/* 任务名字 */
			  (uint16_t       )256,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )4, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&init_task_handle);	/* 任务控制块指针 */ 
	xTaskCreate((TaskFunction_t )task1,  		/* 任务入口函数 */
			  (const char*    )"task1",			/* 任务名字 */
			  (uint16_t       )256,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&task1_handle);	/* 任务控制块指针 */ 
	xTaskCreate((TaskFunction_t )task2,  		/* 任务入口函数 */
			  (const char*    )"task2",			/* 任务名字 */
			  (uint16_t       )256,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&task2_handle);	/* 任务控制块指针 */
	xTaskCreate((TaskFunction_t )task3,  		/* 任务入口函数 */
			  (const char*    )"task3",			/* 任务名字 */
			  (uint16_t       )256,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&task3_handle);	/* 任务控制块指针 */
	xTaskCreate((TaskFunction_t )task4,  		/* 任务入口函数 */
			  (const char*    )"task4",			/* 任务名字 */
			  (uint16_t       )256,  				/* 任务栈大小 */
			  (void*          )NULL,				/* 任务入口函数参数 */
			  (UBaseType_t    )2, 					/* 任务的优先级 */
			  (TaskHandle_t*  )&task4_handle);	/* 任务控制块指针 */
	vTaskStartScheduler(); 
}
