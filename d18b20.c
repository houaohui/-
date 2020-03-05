#include <stc12c5a60s2.h>
sbit DS = P3^5; //1_wire

void delay(unsigned int us)
{
	while(us--);
}

//初始化程序
bit ds_init()
{
	bit i;
	DS=1;
	delay(1);
	DS=0;
	delay(998);//延时480us以上，ds18b20将被复位
	DS=1;
	delay(59);//15~60us,等待
	i=DS;
	delay(300);//采样之后等待60~240us
	DS=1;
	delay(1);
	return (i);
}

//写一个字节
void write_byte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DS=0;
		delay(1);
		DS=dat&0x01;
		delay(150);//延时60us以上
		DS=1; //释放总线准备下次数据写入
		delay(1);//1us
		dat >>= 1;
	}
}
//读一个字节
unsigned char read_byte()
{
	unsigned char i,j,dat;
	for(i=0;i<8;i++)
	{
		DS=0;//产生读时序
		delay(1);//1us
		DS=1;//释放中线采样
		delay(1);//1us
		j=DS;
		delay(150);//60us以上
		DS=1;
		delay(1);//1us
		dat=(j<<7)|(dat>>1);
	}
	return(dat);
}
bit Start18B20()
{
	bit ack;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0x44);
	}
	EA=1;
	return(~ack);
}


bit Get18B20Temp(int *temp)
{
	bit ack;
	unsigned char LSB,MSB;
	EA=0;
	ack=ds_init();
	if(ack==0)
	{
		write_byte(0xcc);
		write_byte(0xbe);
		LSB=read_byte();
		MSB=read_byte();
		*temp=((int)MSB<<8)+LSB;
	}
	EA=1;
	return ~ack;
}