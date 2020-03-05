#include <stc12c5a60s2.h>
#define ADC_POWER 0x80
#define ADC_FLAG 0x10
#define ADC_START 0x08
#define ADC_SPEEDLL 0x00
sfr PLASF = 0x9d;
sbit ENLED=P1^1;
unsigned char code ledchar[]={
	0xC0,0xF9,0xA4,0xB0,
	0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,
	0xC6,0xA1,0x86,0x8E
};
unsigned char xdata ledbuff[4] = {
	0xFF,0xFF,0xFF,0xFF};
unsigned char code keycodemap[4][4] = {
	{ 1, 2, 3, 'u' },
	{ 4, 5, 6, 'l' },
	{ 7, 8, 9, 'd' },
	{ 0, 'e', 'n', 'r' }
};
unsigned char xdata keysta[4][4] = {
 {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}};
void disply();
bit ds_init();
bit Get18B20Temp(int *temp);
bit Start18B20();
void InitInfrared();
void ds18b20();
void ADC_init();
void adcstr();
void keyscan();
void keydriver();
void Timer0Init();
void configuart();
void UARTsend(unsigned char *buf);
void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
void LcdAreaClean(unsigned char x, unsigned char y, unsigned char len);
void UARTdriver();
void LcdFullClean();
void report();
unsigned int GetHighTime();
unsigned int GetLowTime();
void inputmima(unsigned char i);
void fuzhi();
void gaimiaction(unsigned char i);
void gaishijian(unsigned char i);
void IRAction();

unsigned char code start0[] = "Waiting...";
unsigned char code start1[] = "Please input password!";
unsigned char code start2[] = "Welcome to you!";
unsigned char code start3[] = "The password is error! pleease input again!";
unsigned char code gaimima[] = "C changepassword";
unsigned char mima[7]="123456";
unsigned char xdata bufRxd[16],cntRxd=0;
unsigned char xdata shi=23,fen=59,miao=29,sw,ab=0;
unsigned char xdata tshi=0,tfen=1,tmiao=0;//闹钟时分秒
unsigned char xdata sshi=0,sfen=0,smiao=0;//上限设置时间
unsigned char jiabei=1,dat,max=250,min=10;
unsigned int index;
unsigned char xdata ircode[4];
bit en,en2;//蜂鸣器使能和电压上限警报使能
bit ui,uart,flag1s,flag1s2,irflag,disp,flagTxd,yici,yi,gaimi,gaishi;



void main()
{
	ENLED=0;
	EA=1;
	EADC=1;
	ADC_init();
	Timer0Init();
	configuart();
	InitInfrared();
	InitLcd1602();
	UARTsend(start1);
	LcdShowStr(0,0,start0);//显示等待
	while(1)
	{
		UARTdriver();
		if(ui)
		{
			report();//监视上下限函数
			switch(ab)
			{
				case 0:if(disp){disply();disp=0;}ds18b20();adcstr();break;//A界面0.1秒刷新一次
				case 1:if(yi){LcdFullClean();disply();yi=0;}break;//只刷新一次B界面
				case 2:if(yi){LcdShowStr(0,0,gaimima);yi=0;gaimi=1;}break;//改密码界面
				case 3:if(yi){LcdShowStr(0,0,"D Change Time");yi=0;gaishi=1;}//改时间界面
		  }
			switch(sw)//用于B界面的选择时分秒上下限指示
			{
				case 0:break;
				case 1:if(yici){disply();LcdShowStr(2,0,"__");yici=0;}break;
				case 2:if(yici){disply();LcdShowStr(5,0,"__");yici=0;}break;
				case 3:if(yici){disply();LcdShowStr(8,0,"__");yici=0;}break;
				case 4:if(yici){disply();LcdShowStr(3,1,"___");yici=0;}break;
				case 5:if(yici){disply();LcdShowStr(9,1,"___");yici=0;}break;
			}
			
			
		}
		IRAction();//红外处理
		keydriver();//键盘处理
	}
}

void IRAction()
{
	if(irflag)
	{
		irflag=0;
		if(ab==1)//改时分秒专用键盘
		{
				switch(ircode[2])
			{
				case 0x07:sw++;yici=1;if(sw==6)sw=1;break;
				case 0x09:fuzhi();yi=1;break;
				case 0x44:jiabei++;yi=1;if(jiabei==11)jiabei=1;break;
			}
		}
		
		if(ui)//主键盘
		{
			switch(ircode[2])
			{
				case 0x40:ab++;yi=1;sw=0;gaimi=0;gaishi=0;if(ab==4)ab=0;LcdFullClean();break;
				case 0x19:en=0;P13=1;break;
				case 0x45:ui=0;ab=0;LcdFullClean();LcdShowStr(0,0,start0);break;//guanji
			}
		}
		else//关机后的专用键盘
		{
			switch(ircode[2])
			{
				case 0x0c:inputmima(1);break;
				case 0x18:inputmima(2);break;
				case 0x5e:inputmima(3);;break;
				case 0x08:inputmima(4);break;
				case 0x1c:inputmima(5);break;
				case 0x5a:inputmima(6);break;
				case 0x42:inputmima(7);break;
				case 0x52:inputmima(8);;break;
				case 0x4a:inputmima(9);break;
				case 0x16:inputmima(0);break;
			}
		}
		
		if(gaimi)//改密码专用键盘
		{
			switch(ircode[2])
			{
				case 0x0c:gaimiaction(1);break;
				case 0x18:gaimiaction(2);break;
				case 0x5e:gaimiaction(3);;break;
				case 0x08:gaimiaction(4);break;
				case 0x1c:gaimiaction(5);break;
				case 0x5a:gaimiaction(6);break;
				case 0x42:gaimiaction(7);break;
				case 0x52:gaimiaction(8);;break;
				case 0x4a:gaimiaction(9);break;
				case 0x16:gaimiaction(0);break;
			}
		}
		if(gaishi)//改时间专用键盘
		{
			switch(ircode[2])
			{
				case 0x0c:gaishijian(1);break;
				case 0x18:gaishijian(2);break;
				case 0x5e:gaishijian(3);break;
				case 0x08:gaishijian(4);break;
				case 0x1c:gaishijian(5);break;
				case 0x5a:gaishijian(6);break;
				case 0x42:gaishijian(7);break;
				case 0x52:gaishijian(8);;break;
				case 0x4a:gaishijian(9);break;
				case 0x16:gaishijian(0);break;
			}
		}
	}
}

void report()
{
	unsigned char str[12];
	if((dat>max)|(dat<min))
	{
		en2=1;
		dat=(dat*50)/255.0;
		dat=dat*10;
		str[0]=dat/100+'0';
		str[1]='.';
		str[2]=(miao/10%10)+'0';
		str[3]=(dat%10)+'0';
		str[4]='+';
		str[5]=shi/10+'0';
		str[6]=shi%10+'0';
		str[7]=fen/10+'0';
		str[8]=fen%10+'0';
		str[9]=(miao/10%10)+'0';
		str[10]=miao%10+'0';
		str[11]=' ';
		str[12]='\0';
		if(flag1s2)
		{
			flag1s2=0;
			UARTsend(str);
		}
	}
	else
	{
		en2=0;
		P13=1;
	}
}

void disply()
{
	unsigned char abs[12];
	if(ab==0)//要显示界面A
	{
		abs[0]='A';
		abs[1]=' ';
		abs[2]=shi/10+'0';
		abs[3]=shi%10+'0';
		abs[4]='-';
		abs[5]=fen/10+'0';
		abs[6]=fen%10+'0';
		abs[7]='-';
		abs[8]=(miao/10%10)+'0';
		abs[9]=miao%10+'0';
		abs[10]='\0';
		LcdShowStr(0,0,abs);
	}
	if(ab==1)
	{
		abs[0]='B';
		abs[1]=' ';
		abs[2]=tshi/10+'0';
		abs[3]=tshi%10+'0';
		abs[4]='-';
		abs[5]=tfen/10+'0';
		abs[6]=tfen%10+'0';
		abs[7]='-';
		abs[8]=tmiao/10+'0';
		abs[9]=tmiao%10+'0';
		abs[10]=' ';
		abs[11]=jiabei+'0';
		abs[12]='\0';
		LcdShowStr(0,0,abs);
		
		abs[0]='m';
		abs[1]='a';
		abs[2]='x';
		abs[3]=(max/100+'0');
		abs[4]=(max/10%10)+'0';
		abs[5]=(max%10)+'0';
		abs[6]='m';
		abs[7]='i';
		abs[8]='n';
		abs[9]=(min/100+'0');
		abs[10]=(min/10%10)+'0';
		abs[11]=(min%10)+'0';
		abs[12]='\0';
		LcdShowStr(0,1,abs);
	}
}

unsigned char IntToString(unsigned char *str,int dat)
{
	signed char i=0;
	unsigned char len=0;
	unsigned char buf[6];

	if(dat<0)
	{
		dat=-dat;
		*str++='-';
		len++;
	}
	do{
		buf[i++]=dat%10;
		dat/=10;
	}while(dat>0);
	len+=i;
	while(i-->0)
	{
		*str++=buf[i]+'0';
	}
	*str='\0';
	return len;
}

void ds18b20()
{
	int xdata temp;
	int xdata intT,decT;
	unsigned char len;
	unsigned char xdata str[12];
	if(flag1s)
		{
			if(Get18B20Temp(&temp))
			{
				intT=temp>>4;
				decT=temp&0xF;
				len=IntToString(str,intT);
				str[len++]='.';
				decT=(decT*10)/16;
				str[len++]=decT+'0';
				while(len<6)
				{
					str[len++]=' ';
				}
				str[len]='\0';
				LcdAreaClean(0,1,16);
				LcdShowStr(5,1,str);
			}
			Start18B20();
		}
}

bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2)//只用于密码比较
{
	while (*ptr2!='\0')
	{
			if (*ptr1++ != *ptr2++) //遇到不相等数据时即刻返回 0
		{
			return 0;
		}
	}
	return 1; //比较完全部长度数据都相等则返回 1 
}
void UARTsend(unsigned char *buf)
{
	while(*buf!='\0')
	{
		flagTxd=0;
		SBUF=*buf++;
		while(!flagTxd);
	}
}
void uartaction()
{
	if(ui==0)//开机后不执行密码判断
	{
		if( CmpMemory(bufRxd,mima))//比较成功后
		{
			ui=1;
			UARTsend(start2);
		}
		else//没有成功则
		{
			UARTsend(start3);
		}
	}
	
	if(bufRxd[0]=='T')//设置时间
	{
		shi=(bufRxd[1]-'0')*10+(bufRxd[2]-'0');
		fen=(bufRxd[3]-'0')*10+(bufRxd[4]-'0');
		miao=(bufRxd[5]-'0')*10+(bufRxd[6]-'0');
	}
}
void UARTdriver()
{
	if(uart)//一秒以后如果没有接受数据则开始
	{
		if(cntRxd!=0)
		{
			uartaction();
		}
		uart=0;
		cntRxd=0;//下次从头开始接受数据
	}
}
void configuart()
{
	SCON=0x50; //串口模式1
	AUXR=0x11;
	BRT=0xf7;
	ES=1;//使能串口中断
}

void Timer0Init()		//1毫秒@33.1776MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x33;		//设置定时初值
	TH0 = 0xF5;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
}
void ledscan()
{
	static unsigned char i = 0;

	P0 = 0xFF;
	P2 = (P2 & 0xF8) | i;
	P0 = ledbuff[i];
	if (i < 3)
	i++;
	else
	i = 0;
}
void InterruptTimer0() interrupt 1
{
	TL0 = 0x33;
	TH0 = 0xF5;
	keyscan();
	ledscan();
	index++;
	if(index>=1000)
	{
		miao++;
		index=0;
		flag1s=1;
		flag1s2=1;
		uart=1;
	}
	if(index==200)
		disp=1;
	if(miao>=59)
	{
		miao=0;
		fen++;
	}
	if(fen>=59)
	{
		fen=0;
		shi++;
	}
	if(shi>=24)
	{
		shi=0;
	}
	if(tmiao>=59)
	{
		tmiao=0;
	}
	if(tfen>=59)
	{
		tfen=0;
	}
	if(tshi>=24)
	{
		tshi=0;
	}
	if((miao==tmiao)&&(shi==tshi)&&(fen==tfen))
	{
		en=1;
	}
	
	if((en)|(en2))
	{
		P13=~P13;
	}
}

void interruptuart()  interrupt 4
{
	if(RI)
	{
		RI=0;
		bufRxd[cntRxd++]=SBUF;
		index=0;
		flag1s=0;
	}
	if(TI)
	{
		TI=0;
		flagTxd=1;
	}
}

void ADC_init()
{
	PLASF = 0x01;
	ADC_RES = 0;
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START;
}

void adc() interrupt 5 using 1
{
	ADC_CONTR&=!ADC_FLAG;
	dat = ADC_RES;
	ledbuff[2]=ledchar[dat%10];
	ledbuff[1]=ledchar[dat/10%10];
	ledbuff[0]=ledchar[dat/100%10];
	
	ADC_CONTR = ADC_POWER|ADC_SPEEDLL|ADC_START;
}
void adcstr()
{
	unsigned char str[5];
	if(flag1s)
	{
		flag1s=0;
		dat=(dat*50)/255.0;
		dat=dat*10;
		str[0]=dat/100+'0';
		str[1]='.';
		str[2]=(miao/10%10)+'0';
		str[3]=(dat%10)+'0';
		str[4]='\0';
		LcdShowStr(0,1,str);
	}
}
void fuzhi()
{
	switch(sw)
	{
		case 1:tshi=tshi+jiabei;break;
		case 2:tfen=tfen+jiabei;break;
		case 3:tmiao=tmiao+jiabei;break;
		case 4:max=max+jiabei;break;
		case 5:min=min+jiabei;break;
	}
}
void gaimiaction(unsigned char i)
{
	static unsigned char steap=0;
	unsigned char s[2];
	s[1]='\0';
	s[0]=mima[steap]=(i+'0');
  LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		steap=0;
		LcdAreaClean(0,1,16);
		LcdShowStr(0,1,"OK");
	}
}
void inputmima(unsigned char i)
{
	static unsigned char steap=0;
	static unsigned char str[7],s[2];
	str[7]='\0';
	s[1]='\0';
	s[0]=str[steap]=(i+'0');
	LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		steap=0;
		LcdAreaClean(0,1,16);
		if(CmpMemory(str, mima))
		{
			ui=1;
		}
		else
		{
			LcdAreaClean(0,0,16);
			LcdShowStr(0,0,"Try again!");
		}
	}
}
void gaishijian(unsigned char i)
{
	static unsigned char steap=0;
	unsigned char s[2],str[6];
	s[1]='\0';
	s[0]=str[steap]=(i+'0');
  LcdShowStr(steap,1,s);
	steap++;
	if(steap==6)
	{
		shi=(str[0]-'0')*10+(str[1]-'0');
		fen=(str[2]-'0')*10+(str[3]-'0');
		miao=(str[4]-'0')*10+(str[5]-'0');
		steap=0;
		LcdAreaClean(0,1,16);
		LcdShowStr(0,1,"OK");
	}
}
void keyaction(unsigned char keycode)
{
	if(ab==1)//改时分秒专用键盘
	{
			switch(keycode)
		{
			case 'l':sw++;yici=1;if(sw==6)sw=1;break;
			case 'r':fuzhi();yi=1;break;
			case 'n':jiabei++;yi=1;if(jiabei==11)jiabei=1;break;
		}
	}
	
	if(ui)//主键盘
	{
		switch(keycode)
		{
			case 'u':ab++;yi=1;sw=0;gaimi=0;gaishi=0;if(ab==4)ab=0;LcdFullClean();break;
			case 'd':en=0;P13=1;break;
			case 'e':ui=0;ab=0;LcdFullClean();LcdShowStr(0,0,start0);break;//guanji
		}
	}
	else//关机后的专用键盘
	{
		switch(keycode)
		{
			case 1:inputmima(1);break;
			case 2:inputmima(2);break;
			case 3:inputmima(3);;break;
			case 4:inputmima(4);break;
			case 5:inputmima(5);break;
			case 6:inputmima(6);break;
			case 7:inputmima(7);break;
			case 8:inputmima(8);;break;
			case 9:inputmima(9);break;
			case 0:inputmima(0);break;
		}
	}
	
	if(gaimi)//改密码专用键盘
	{
		switch(keycode)
		{
			case 1:gaimiaction(1);break;
			case 2:gaimiaction(2);break;
			case 3:gaimiaction(3);;break;
			case 4:gaimiaction(4);break;
			case 5:gaimiaction(5);break;
			case 6:gaimiaction(6);break;
			case 7:gaimiaction(7);break;
			case 8:gaimiaction(8);;break;
			case 9:gaimiaction(9);break;
			case 0:gaimiaction(0);break;
		}
	}
	if(gaishi)//改时间专用键盘
	{
		switch(keycode)
		{
			case 1:gaishijian(1);break;
			case 2:gaishijian(2);break;
			case 3:gaishijian(3);break;
			case 4:gaishijian(4);break;
			case 5:gaishijian(5);break;
			case 6:gaishijian(6);break;
			case 7:gaishijian(7);break;
			case 8:gaishijian(8);;break;
			case 9:gaishijian(9);break;
			case 0:gaishijian(0);break;
		}
	}
}

		
void keydriver()
{
	unsigned char i, j;
	static unsigned char xdata backup[4][4] = {
	{1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}
	};
	for (i=0; i<4; i++)
	{
		for (j=0; j<4; j++)
		{
			if (backup[i][j] !=keysta[i][j])
			{
				if (backup[i][j] == 0)
				{
					keyaction(keycodemap[i][j]);
				}
				backup[i][j] = keysta[i][j];
			}
		}
	}
}

void keyscan()
{
	unsigned char i;
	static unsigned char keyout=0;
	static unsigned char xdata keybuf[4][4]={
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},
	{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}
	};
	keyout=keyout&0x03;
	switch(keyout)
	{
		case 0: P2=0x7f;break;
		case 1: P2=0xbf;break;
		case 2: P2=0xdf;break;
		case 3: P2=0xef;break;
		default:break;
	}
	keybuf[keyout][0]=(keybuf[keyout][0]<<1)|P20;
	keybuf[keyout][1]=(keybuf[keyout][1]<<1)|P21;
	keybuf[keyout][2]=(keybuf[keyout][2]<<1)|P22;
	keybuf[keyout][3]=(keybuf[keyout][3]<<1)|P23;
	for(i=0;i<4;i++)
	{
		if((keybuf[keyout][i]&0x0f)==0x00)
		{
			keysta[keyout][i]=0;
		}
		else if((keybuf[keyout][i]&0x0f)==0x0f)
		{
			keysta[keyout][i]=1;
		}
	}
	keyout++;
}


void EXINT0_IR() interrupt 0
{
	unsigned char i, j;
	unsigned int time;
	unsigned char byt;

	time = GetLowTime();
	if((time <23500) || (time > 26265))	   
	{
		IE0 = 0;
		return;	
	}

	time = GetHighTime();
	if((time<11059) || (time > 13824))
	{
		IE0 = 0;
		return;
	}
	for(i=0; i<4; i++)
	{
		for(j=0; j<8; j++)
		{
			time = GetLowTime();
			if((time<940) ||(time >2157))
			{
				IE0 = 0;
				return;
			}
			time = GetHighTime();
			if((time>940) && (time <2157))
			{
				byt >>= 1;	
			}
			else if((time>4037) && (time<5253))
			{
				byt >>= 1;
				byt |= 0x80;
			}
			else
			{
				IE0 = 0;
				return;
			}
		}
		ircode[i] = byt;
	}
	IE0 = 0;
	irflag=1;
}
