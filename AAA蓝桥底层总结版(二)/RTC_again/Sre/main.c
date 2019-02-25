/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��18��
* ��  ע���޸ĺ��LCD����
*         RTC��ʾ��LCD�ϣ���ע�⵽24:00:00�����⴦��
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

u8 RTC_Flag = 0;
u32 time = 0;

u8 hour, min, sec;

//Main Body
int main(void)
{
	u8 str[20];
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	RTC_Init(23, 59, 55);
	
	while(1)
	{
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			time = RTC_GetCounter();
			RTC_WaitForLastTask();
			
			if(time == 24 * 3600)
			{
				RTC_SetCounter(1);//���⴦���£������ڵ�0��ʱ�򿨶١�
				RTC_WaitForLastTask();
				LCD_DisplayStringLine(Line5, "     00:00:00   ");
			}
			else
			{
				hour = time / 3600;
				min = time % 3600 / 60;
				sec = time % 3600 % 60;
				sprintf((char *)str, "     %.2d:%.2d:%.2d   ", hour, min, sec);
				LCD_DisplayStringLine(Line5, str);
			}			
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
