/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��18��
* ��  ע���޸ĺ��LCD����
*         PCͨ��USART2�������ݵ���Ƭ��������ʾ��LCD��(PA2-TXD PA3-RXD) 
*		  ע�ⷢ��������\r\n���޳�\r��ʾ����Ļ�ϵ�����Ӱ��
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

u32 TimingDelay = 0;
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
void Delay_Ms(u32 nTime);
u8 RTC_Flag = 0;

//Main Body
int main(void)
{
	u8 i;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);

	USART2_Init();
	
	while(1)
	{
		if(RxdOver)
		{
			RxdOver = 0;
			LCD_ClearLine(Line5);//���LCD�Ķ�Ӧ��
			LCD_DisplayStringLine(Line5, RxdBuf);
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
			for(i=0; i<20; i++)//��ջ�����
				RxdBuf[i] = 0;
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
