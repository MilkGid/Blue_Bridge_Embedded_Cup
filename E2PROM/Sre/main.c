/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��17��
* ��  ע���޸ĺ��LCD����
*         E2PR0M��0x01�����ַд������0xAA�������ٶ�����LCD
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 str[20];
	u8 dat;

	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	i2c_init();//FUCK,�����������ʼ��
	E2Write(0x01, 0xAA);//��0x01�����ַд��,0xAA�������
	Delay_Ms(5);
	dat = E2Read(0x01);
	
	LCD_ClearLine(Line5);
	sprintf((char *)str, "data = %d", dat);
	LCD_DisplayStringLine(Line5, str);
	
	while(1);
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
