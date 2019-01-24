/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��24��
* ��  ע������B1����LED1������
*         
*******************************************************************************
*/

#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "timer.h"


int main(void)
{
	LEDInit();
	KeyInit();
	TIM2Init(2000, 72);//��ʱ2ms
	
	while(1)
	{	
		KeyDriver();
	}
}

void KeyAction(int code)
{
	if(code == 1)
	{
		GPIOC->ODR ^= (1<<8);//PC8����ȡ��
		GPIOD->ODR |= (1<<2);//PD2��1��ʹ��573������
		GPIOD->ODR &= ~(1<<2);//PD2��0���ر�573������
	}
}


