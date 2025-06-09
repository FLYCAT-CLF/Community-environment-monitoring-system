#ifndef __OLED_H_
#define __OLED_H_
#define OLED_ADDRESS	0x78 
void OLED_init(void);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_CLS(void);
void OLED_Fill(unsigned char fill_Data);
void OLED_SetPos(unsigned char x, unsigned char y);
#endif
