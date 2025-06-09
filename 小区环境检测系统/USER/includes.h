#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "key.h"
#include "bmp.h"
#include "OLED.h"
#include "FreeRTOS.h"
#include "task.h"
#include "IIC.h"
#include "adc.h"
#include "RTC.h"
#include "dht11.h"
#include "esp8266.h"
#include "time.h"
#include "semphr.h"
#include "cjson.h"
#include "esp8266_mqtt.h"
#include "led.h"
#include "LX-MP3.h"
#include "queue.h"
extern SemaphoreHandle_t  g_sem_binary;
extern QueueHandle_t  g_queue;
//extern GPIO_InitTypeDef  	GPIO_InitStructure;
//extern NVIC_InitTypeDef 	NVIC_InitStructure;		
//extern SPI_InitTypeDef  	SPI_InitStructure;

#endif
