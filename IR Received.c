#include <STC12C5A60S2.H>
sbit IR_INPUT = P3^2; 

void InitInfrared()
{	
	IR_INPUT = 1;
	TMOD &= 0X0F;
	TMOD |= 0x10;
	TR1 = 0;
	ET1 = 0;
	IT0 = 1;
	EX0 = 1;
}
unsigned int GetHighTime()
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	while(IR_INPUT)
	{
		if(TH1 > 0xC2)
		{
			break;
		}
	}
	TR1 = 0;

	return(TH1 * 256 + TL1);
}

unsigned int GetLowTime()
{
	TH1 = 0;
	TL1 = 0;
	TR1 = 1;
	while(!IR_INPUT)
	{
		if(TH1 > 0xC2)
		{
			break;
		}
	}
	TR1 = 0;

	return(TH1 * 256 + TL1);
}
