#include<reg52.h>			//52系列单片机头文件
#include<intrins.h>			 //包含_nop_函数所在的头文件
#define uint unsigned int	  //宏定义
#define uchar unsigned char
sbit CS=P0^5;					 //ADC0804 CS 端
sbit wr=P0^7;					 //ADC0804 WR 端
sbit rd=P0^6;					 //ADC0804 RD 端
sbit wei1=P0^0;				 //位选端
sbit wei2=P0^1;
sbit wei3=P0^2;
sbit wei4=P0^3;
sbit fan = P3^2;
sbit pure = P3^3;
sbit ledPower = P0^4;
uchar a,A1,A2,A3,pm,count; 
uint pmA;
uchar code table[]={
								0xC0,  //"0"
                0xF9,  //"1"
                0xA4,  //"2"
                0xB0,  //"3"
                0x99,  //"4"
                0x92,  //"5"
                0x82,  //"6"
                0xF8,  //"7"
                0x80,  //"8"
                0x90,  //"9"
                0x88,  //"A"
                0x83,  //"B"
                0xC6,  //"C"
                0xA1,  //"D"
                0x86,  //"E"
                0x8E,  //"F"
                0x89,  //"H"
                0xC7,  //"L"
                0xC8,  //"n"
                0xC1,  //"u"
                0x8C,  //"P"
                0xA3,  //"o"
                0xBF,  //"-"
                0xFF,  //熄灭
								};


void delay(uint xms)			 //延时函数
{
  uint i,j;
  for(i=xms;i>0;i--)
  for(j=110;j>0;j--);
}

void delay4us(uint xus)   //误差 0us
{
		uint i;
		for(i=xus;i>0;i--){
			_nop_();  
		}
}

void display()	 //数码管显示函数
{
   P1 = table[pmA/1000];
   wei1 = 0;
   delay(5);
   wei1 = 1;

   P1 = table[pmA%1000/100];
   wei2 = 0;
   delay(5);
   wei2 = 1;

   P1 = table[pmA%100/10];
   wei3 = 0;
   delay(5);
   wei3 = 1;

   P1 = table[pmA%10];
   wei4 = 0;
   delay(5);
   wei4 = 1;
  
}

void main()			//主程序
{
	TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
	TH0 = 0x0DA;	      //给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
	TL0 = 0x30;
	EA=1;            //总中断打开
	ET0=1;           //定时器中断打开
	TR0=1;           //定时器开关打开
	
  CS=0;
	fan = 0;
  while(1)
  {	
  wr=1;
	_nop_();
	wr=0;			 	//启动A/D转换
	_nop_();
	wr=1;
  for(a=10;a>0;a--)
  {	
	   display();	
	}
	if(pmA >= 3000){
		fan = 1;
    pure = 1;
	}
	if(pmA <= 3000){
		fan = 0;
    pure = 0;
	}
  }
}
void Timer0_isr(void) interrupt 1 using 1
{
	TH0 = 0x0DA;		  //重新赋值，方式1是16位计数器，不能硬件重装初始值
	TL0 = 0x30;
	count++;
	if(count == 500)
	{
		count = 0;
	}
	ledPower = 0;
	delay4us(70); //delay 280us
	
	P2 = 0xff;			//读取P2口之前先给其写全1
  rd = 1;				//选通AD0804 CS 端
	_nop_();
  rd = 0;				//A/D读使能
	_nop_();
	if(count == 1){
  pm = P2;			//A/D数据读取赋给P2口
	pmA = pm *39;
	}
	rd = 1;
  delay4us(8); //delay 32us
	ledPower = 1;
}