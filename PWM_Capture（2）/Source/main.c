/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��1��
* ��  ע�����������ڲ����ⲿ�����źŵ�Ƶ��
*         
*******************************************************************************
*/

#include "config.h"

u8 str[20];
extern __IO uint32_t TIM3Freq;

int main(void)
{
	u32 i = 0;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);//��������һ���������
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	TIM3_InputCaptureInit();
	
	while(1)
	{
		sprintf((char *)str, "TIM3Freq = %d           ", TIM3Freq);
		LCD_DisplayStringLine(Line1, str);
		for(i=0; i<500000; i++);
	}
}

