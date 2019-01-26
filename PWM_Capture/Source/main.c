/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��26��
* ��  ע��TIM3_CH1-PA6, TIM3_CH2-PA7����Ϊ���벶��
*         ����TIM4��CH1-PB6��PB7��Ƶ���Լ�ռ�ձ�
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "lcd.h"
#include "stdio.h"
#include "usart.h"
#include "i2c.h"
#include "eeprom.h"
#include "PWMMode_Advance.h"
#include "PWM_Compare.h"
#include "PWM_Capture.h"


u8 string[20];

int main(void)
{
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//��ʱ2ms
	
	TIM1_PWMInit(2000, 40, 80);//Ƶ��2K CH2ռ�ձ�40% CH3ռ�ձ�80%
	TIM4_PWMCompare(2000, 1000, 40, 80);//CH1ռ�ձ�2K 40% CH2 1Kռ�ձ�80%
	
	TIM3_CaptureInit();
	
	while(1)
	{	
		KeyDriver();
		
		if(TIM3_CH1_CAPTURE_MODE == 3)
		{
			sprintf((char*)string,"ch1_fre:%d           ",1000000 / TIM3_CH1_CAPTURE_HL);
			LCD_DisplayStringLine(Line3, string);
			sprintf((char*)string,"ch1_duty:%d          ",TIM3_CH1_CAPTURE_H * 100/TIM3_CH1_CAPTURE_HL);
			LCD_DisplayStringLine(Line4, string);
			TIM3_CH1_CAPTURE_MODE = 0;
		}

		if(TIM3_CH2_CAPTURE_MODE == 3)
		{
			sprintf((char*)string,"ch2_fre:%d           ",1000000 / TIM3_CH2_CAPTURE_HL);
			LCD_DisplayStringLine(Line7, string);
			sprintf((char*)string,"ch2_duty:%d          ",TIM3_CH2_CAPTURE_H * 100/TIM3_CH2_CAPTURE_HL);
			LCD_DisplayStringLine(Line8, string);
			TIM3_CH2_CAPTURE_MODE = 0;
		}
	}
}

void KeyAction(int code)
{
	if(code == 1)//����B1���л���״̬������������0.1s
	{
		GPIOC->ODR ^= (1<<8);//PC8����ȡ��
		GPIOD->ODR |= (1<<2);//PD2��1��ʹ��573������
		GPIOD->ODR &= ~(1<<2);//PD2��0���ر�573������
		Beep(100);
	}
	else if(code == 2)
	{
		Beep(-1);
	}
	else if(code == 3)
	{
		Beep(0);
	}
	else if(code == 4)
	{
		
	}
}


