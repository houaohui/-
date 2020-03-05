#include <stc12c5a60s2.h>
#define LCD1602_DB P0
sbit LCD1602_RS = P2^0;
sbit LCD1602_RW = P2^1;
sbit LCD1602_E = P1^2;
void delay()//300ns
{}

void LcdWaitReady()
{
 unsigned char sta;
 
 LCD1602_DB = 0xFF;
 LCD1602_RS = 0;
 LCD1602_RW = 1;
 do {
 LCD1602_E = 1;
 sta = LCD1602_DB; 
	delay();
 LCD1602_E = 0;
 } while (sta & 0x80); 
}

void LcdWriteCmd(unsigned char cmd)
{
 LcdWaitReady();
 LCD1602_RS = 0;
 LCD1602_RW = 0;
 LCD1602_DB = cmd;
	delay();
 LCD1602_E = 1;
	delay();
 LCD1602_E = 0;
	delay();
}

void LcdWriteDat(unsigned char dat)
{
 LcdWaitReady();
 LCD1602_RS = 1;
 LCD1602_RW = 0;
 LCD1602_DB = dat;
	delay();
 LCD1602_E = 1;
	delay();
 LCD1602_E = 0;
	delay();
}

void LcdSetCursor(unsigned char x, unsigned char y)
{
 unsigned char addr;
 EA=0;
 if (y == 0) //
 addr = 0x00 + x; //
 else
 addr = 0x40 + x; //
 LcdWriteCmd(addr | 0x80); //
 EA=1;
}

void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str)
{
	EA=0;
 LcdSetCursor(x, y); //
 while (*str!='\0') //
 {
 LcdWriteDat(*str++);
 }
 EA=1;
}
void LcdAreaClean(unsigned char x, unsigned char y, unsigned char len)
 {
	 EA=0;
	 LcdSetCursor(x,y);
	 while(len--)
	 {
		 LcdWriteDat(' ');
	 }
	 EA=1;
 }
 void LcdFullClean()
 {
	 EA=0;
	 LcdWriteCmd(0x01);
	 EA=1;
 }

void InitLcd1602()
{
 LcdWriteCmd(0x38); //
 LcdWriteCmd(0x0C); //开光标不闪烁
 LcdWriteCmd(0x06); 
 LcdWriteCmd(0x01); 
}