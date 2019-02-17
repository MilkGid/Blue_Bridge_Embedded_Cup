/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��17��
* ��  ע���޸ĺ��LCD����
*         �����Ӧ��PB0(ADC1_IN8)ADֵ������Ϊ0~3.3V��ʾ��LCD��
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 str[20];
	u8 dat;
	float dat_ADC;

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
	
	ADC1_Init();
	
	
	while(1)
	{
		dat_ADC = Get_ADC();
		sprintf((char *)str, "ADC = %.2f V", dat_ADC * 3.3 / 4096);
		LCD_DisplayStringLine(Line2, str);
		Delay_Ms(500);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
