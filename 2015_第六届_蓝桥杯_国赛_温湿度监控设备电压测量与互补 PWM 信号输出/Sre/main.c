/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��23��
* ��  ע���޸ĺ��LCD����
*         2015_������_���ű�_����_��ʪ�ȼ���豸��ѹ�����뻥�� PWM �ź����
*		  ʵ���������й���
*		  ���Ӱ���������
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
#include "beep.h"

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);


//ȫ�ֱ���
u8 flagSet = 0;//�������ý����ʵʱ��ʾ�����л�
u8 str[20];//�ַ������õ�����
u8 flag200ms = 1;//200ms��ʱ����־

//ADC����
float ADC_Val;

//PWM���
u8 OUT_Sta = 0;//0-STOP 1-START
u8 Show_CH2_Duty = 0;//PA9��Ӧռ�ձ�
u8 Show_CH2N_Duty = 0;//PB14��Ӧռ�ձ�
u8 Freq = 1;//�źŶ�Ӧ��Ƶ�����

//LED
u16 LED_MODE = 0XFFFF;

void ShowRealData(void);
void ShowSetDat(void);
void Refresh(void);

//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);

	/*��ʼ������*/
	TIM4_Init(2000, 72);//��ʱ2ms
	KeyInit();
	BeepInit();
	ADC1_Init();
	LED_Init();//LED����
	i2c_init();
	
	PWM_OC_Init(Freq, Show_CH2_Duty, 1, 1);
	
	Freq = E2Read(0x00);//��ȡE2�е�����
	Delay_Ms(5);
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			
			Refresh();
			
			if(!flagSet)
			{
				ShowRealData();//ʵʱ������ʾ����
			}
			else
			{
				ShowSetDat();//�������ý���
			}
		}
	}
}

void Refresh(void)//ˢ����ʾ�������ز���
{
	ADC_Val = Get_ADC() * 3.3 / 4096;//�ɼ�ADCֵ
	Show_CH2_Duty = ADC_Val * 100 / 3.3; //PA9��Ӧ��ռ�ձ�
	Show_CH2N_Duty = 100 - Show_CH2_Duty; //PB14��Ӧ��ռ�ձ�
	
	if(OUT_Sta)
	{
		PWM_OC_Init(Freq, Show_CH2_Duty, 0, 1);
	}
	else
	{
		PWM_OC_Init(Freq, Show_CH2_Duty, 0, 0);
	}
	
	
}

void ShowRealData(void)//��ʾʵʱ���ݽ���
{	

	LCD_DisplayStringLine(Line0, (u8 *)"     Real Data     ");
	
	sprintf((char *)str, " ADC_Val:  %.2fV         ", ADC_Val);
	LCD_DisplayStringLine(Line2, str);
	
	if(OUT_Sta == 0)
		LCD_DisplayStringLine(Line4, (u8*)" OUT_Sta:  STOP        ");
	else
		LCD_DisplayStringLine(Line4, (u8*)" OUT_Sta:  START       ");
	
	sprintf((char *)str, " Signal :  PA9 :%d%%       ", Show_CH2_Duty);
	LCD_DisplayStringLine(Line6, str);
	
	sprintf((char *)str, "           PB14:%d%%      ", Show_CH2N_Duty);
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "           %dKHz        ", Freq);
	LCD_DisplayStringLine(Line8, str);
	
	LCD_DisplayStringLine(Line9, (u8*)"                  1  ");
}

void ShowSetDat(void)//�������ý���
{
	LCD_DisplayStringLine(Line0, (u8 *)"     Set Data      ");
	
	sprintf((char *)str, " Freq:    %dKHz        ", Freq);
	LCD_DisplayStringLine(Line4, str);
	
	LCD_DisplayStringLine(Line9, (u8*)"                  2  ");
}

void KeyAction(int code)
{
	if(code == 1)
	{
		OUT_Sta ^= 1;//�ı�PWM���״̬
		
		if(OUT_Sta)
		{
			LED_MODE &= ~(1<<8);//����״̬�£�����LED1
		}
		else
		{
			LED_MODE |= (1<<8);//�ر�LED1
		}
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
		Beep(100);//����ε�һ��
	}
	else if(code == 2)//B2
	{
		flagSet ^= 1;//�л�����
		if(flagSet == 1)
			LCD_Clear(Blue);
		else//�˳����ý���ǰ��E2�д�Freq
		{
			E2Write(0x00, Freq);
			Delay_Ms(5);
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 3)//��
	{
		if(flagSet)//�������ý�������Ч
		{
			Freq += 1;
			if(Freq > 10)
				Freq = 1;
			Beep(100);//����ε�һ��
		}
	}
	else if(code == 4)//��
	{
		
		Beep(100);//����ε�һ��
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
