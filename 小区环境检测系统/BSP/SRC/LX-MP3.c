#include "includes.h"             
void MP3_init()
{
	RCC->AHB1ENR|=0x3<<3;
	GPIOE->MODER|=0x15555<<(2*7);
	GPIOE->OTYPER=0xff8<<7;
	GPIOE->ODR|=0xFF80;
}
void lx_figure(u8 a)
{
	switch(a)
	{
		case 1:
			GPIOE->ODR&=~0x80;
			break;
		case 2:
			GPIOE->ODR&=~0x100;
			break;
		case 3:
			GPIOE->ODR&=~0x180;
			break;
		case 4:
			GPIOE->ODR&=~0x200;
			break;
		case 5:
			GPIOE->ODR&=~0x280;
			break;
		case 6:
			GPIOE->ODR&=~0x300;
			break;
		case 7:
			GPIOE->ODR&=~0x380;
			break;
		case 8:
			GPIOE->ODR&=~0x400;
			break;
		case 9:
			GPIOE->ODR&=~0x480;
			break;
		case 10:
			GPIOE->ODR&=~0x500;
			break;
		case 11:
			GPIOE->ODR&=~0x580;
			break;
		case 12:
			GPIOE->ODR&=~0x600;
			break;
		case 13:
			GPIOE->ODR&=~0x680;
			break;
		case 14:
			GPIOE->ODR&=~0x700;
			break;
		case 15:
			GPIOE->ODR&=~0x780;
			break;
		case 16:
			GPIOE->ODR&=~0x800;
			break;
		case 17:
			GPIOE->ODR&=~0x880;
			break;
		case 18:
			GPIOE->ODR&=~0x900;
			break;
		case 19:
			GPIOE->ODR&=~0x980;
			break;
		case 20:
			GPIOE->ODR&=~0xA00;
			break;
		case 21:
			GPIOE->ODR&=~0xA80;
			break;
		case 22:
			GPIOE->ODR&=~0xB00;
			break;
		case 23:
			GPIOE->ODR&=~0xB80;
			break;
		case 24:
			GPIOE->ODR&=~0xC00;
			break;
		case 25:
			GPIOE->ODR&=~0xC80;
			break;
		case 26:
			GPIOE->ODR&=~0xD00;
			break;
		case 27:
			GPIOE->ODR&=~0xD80;
			break;
		case 28:
			GPIOE->ODR&=~0xE00;
			break;
		case 29:
			GPIOE->ODR&=~0xE80;
			break;
		case 30:
			GPIOE->ODR&=~0xF00;
			break;
		case 0:
			GPIOE->ODR&=~0xF80;
			break;
	}
	delay_ms(60);
	GPIOE->ODR|=0xF80;
	delay_ms(55);
}
int lx_char(char* a)
{//11-29
	char *s[]={"小区内温度","摄氏度","光照强度百分之","相对湿度百分之","晴",\
								"多云","少云","阴","雨","雪","大风","冰雹","不宜出门",\
								"适宜出门","年","月","日","点","分","天气" };
	for(u8 i=0;i<20;i++)
		if(strcmp(a,s[i])==0)
		{
			lx_figure(11+i);
		return 1;
		}
	for(u8 i=0;i<4;i++)
		if(strcmp(a,s[8+i])==0)
				return 0;
	return -1;
}
