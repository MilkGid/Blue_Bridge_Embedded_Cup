/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��26��
* ��  ע���߼���ʱ��PWM��� PA9-TIM_CH2 PA10-TIM_CH3
*         ��ͨ������4����ռ�ձȣ���ʼռ�ձ�Ϊ40%��ÿ������10%
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
#include "PWMMode.h"
#include "PWMMode_Advance.h"

int main(void)
{
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//��ʱ2ms

	TIM3_PWMInit(1000, 40, 80);//Ƶ��2K CH1ռ�ձ�40% CH2ռ�ձ�80%
	TIM1_PWMInit(1000, 40, 80);//Ƶ��2K CH2ռ�ձ�40% CH3ռ�ձ�80%
	
	while(1)
	{	
		KeyDriver();
	}
}

void KeyAction(int code)
{
	static u32 x = 400;
	
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
		x += 100;
		TIM_SetCompare2(TIM1, x);//�ı�TIM1_CH2ռ�ձ�
	}
}


