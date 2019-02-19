/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��19��
* ��  ע���޸ĺ��LCD����
*         ���ö�ʱ��ʵ�ֵļ�1s��ʱ������ʾ��LCD��
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"
#include "pwm.h"
#include "pwm_oc.h"
#include "pwm_ic.h"
#include "timer.h"

u32 TimingDelay = 0;
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
void Delay_Ms(u32 nTime);
u8 RTC_Flag = 0;
u32 tmr = 0;

//Main Body
int main(void)
{	
	u8 str[20];
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	//PWM_Init(500, 60);//500Hz 60%����//PA1
	PWM_OC_Init(500, 60);//500Hz 60%���� PA1
	PWM_IC_Init();//PA7
	
	TIM4_Init(10000, 7200);//��ʱ1s
	
	while(1)
	{
		sprintf((char *)str, "     %d    ", tmr);
		LCD_DisplayStringLine(Line5, str);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
