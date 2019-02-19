/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��19��
* ��  ע���޸ĺ��LCD����
*         ��ʱ��ʵ��2ms��ʱ�ж�;����B1��LED1״̬�ı�
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
#include "led.h"
#include "key.h"

u32 TimingDelay = 0;
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
void Delay_Ms(u32 nTime);
u8 RTC_Flag = 0;

//Main Body
int main(void)
{
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	//PWM_Init(500, 60);//500Hz 60%����//PA1
	PWM_OC_Init(500, 60);//500Hz 60%���� PA1
	PWM_IC_Init();//PA7
	
	TIM4_Init(2000, 72);//��ʱ2ms
	LED_Init();//LED����
	KeyInit();
	
	while(1)
	{
		KeyDriver();
	}
}

void KeyAction(int code)
{
	if(code == 1)//����B1��LED1״̬�ı�
	{
		GPIOD->ODR |= (1<<2);//ʹ��573
		GPIOC->ODR ^= (1<<8);//LED1�ı�
		GPIOD->ODR &= (1<<2);//ʧ��573
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
