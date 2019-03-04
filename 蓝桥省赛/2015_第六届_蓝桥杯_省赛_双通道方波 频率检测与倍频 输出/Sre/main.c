/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��3��2��
* ��  ע���޸ĺ��LCD����
*         2015_������_���ű�_ʡ��_˫ͨ������ Ƶ�ʼ���뱶Ƶ ���
*		  PWM�����覴ã�����ԭ�����...
*		  ����ͨ�� PA1-TIM2_CH2 ��Ӧ��� PA6-TIM3_CH1
*		  ����ͨ�� PA2-TIM2_CH3 ��Ӧ��� PA7_TIM3_CH2
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
u8 str[20];//�ַ������õ�����
u8 flag200ms = 1;//200ms��ʱ����־
u8 setMode = 0;//����ģʽ: 0-�����趨 1-�����趨
u8 selChannel = 0; //ѡ������ͨ�� 0-����ͨ��1 2-����ͨ��1
u8 flag1s = 0;

//������ر���
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;

//�źŲ�����ر���
extern u32 TIM2_CH2_CAPTURE_HL;
extern u8 TIM2_CH2_CAPTURE_MODE;
extern u32 TIM2_CH3_CAPTURE_HL;
extern u8 TIM2_CH3_CAPTURE_MODE;

//�ź������ر���
u8 Channal_1_Enable = 1;
u8 Channal_2_Enable = 1;
u32 Channal_1_Frequency = 0;
u32 Channal_2_Frequency = 0;

u8 CH1_N = 2;//ͨ��1��Ƶ��
u8 CH2_N = 2;//ͨ��2��Ƶ��
u8 CH1_NBuf = 2;
u8 CH2_NBuf = 2;

//��������
void UartAction(void);
void ShowRealData(void);
void ShowSetData(void);
void RefreshLed(void);
void RefreshN(void);

//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);

	TIM4_Init(2000, 72);//��ʱ2ms
	KeyInit();
	BeepInit();
	ADC1_Init();
	LED_Init();//LED����
	USART2_Init();
	i2c_init();
	
	PWM_IC_Init();//PWM���벶��
	TIM3_OUTPUT_Init(1000, 500, 1, 1, 1);//PWM���
	
	CH1_N = E2Read(0x00);
	Delay_Ms(5);
	CH2_N = E2Read(0x01);
	Delay_Ms(5);
	
	while(1)
	{
		KeyDriver();
		
		
		if(TIM2_CH2_CAPTURE_MODE == 2)
		{
			Channal_1_Frequency = 1000000 / TIM2_CH2_CAPTURE_HL;
			if((Channal_1_Frequency < 50) || (Channal_1_Frequency > 50000))
			{
					Channal_1_Enable = 0;
			}else
			{
					Channal_1_Enable = 1;
			}
			TIM2_CH2_CAPTURE_MODE = 0;
		}
		if(TIM2_CH3_CAPTURE_MODE == 2)
		{
				Channal_2_Frequency = 1000000 / TIM2_CH3_CAPTURE_HL;
				if((Channal_2_Frequency < 50) || (Channal_2_Frequency > 50000))
				{
						Channal_2_Enable = 0;
				}else
				{
						Channal_2_Enable = 1;
				}
				TIM2_CH3_CAPTURE_MODE = 0;
		}
		
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshLed();
			ShowRealData();

			RefreshN();//ˢ�±�Ƶ��
		}
		if(flag1s)
		{
			flag1s = 0;
			TIM3_OUTPUT_Init(Channal_1_Frequency*CH1_N, Channal_2_Frequency*CH2_N, 0, Channal_1_Enable, Channal_2_Enable);
		}
		
		if(setMode == 1)
		{
			if(RxdOver)//���յ��Ժ���\n���ַ���
			{
				RxdOver = 0;
				UartAction();//ִ����Ӧ�Ĵ��ڶ���
			}
		}
	}
}

void RefreshN(void)
{
	if(CH1_N != CH1_NBuf)
	{
		CH1_NBuf = CH1_N;
		E2Write(0x00, CH1_NBuf);
		Delay_Ms(5);
	}
	if(CH2_N != CH2_NBuf)
	{
		CH2_NBuf = CH2_N;
		E2Write(0x01, CH2_NBuf);
		Delay_Ms(5);
	}
}

void RefreshLed(void)//������Ӧ��LED״̬
{
	if(Channal_1_Enable) 
	{
		LED_MODE &= ~(1<<8); 
	}
	else 
	{
		LED_MODE |= (1<<8); 
	}
	
	if(Channal_2_Enable)
	{
		LED_MODE &= ~(1<<9); 
	}
	else
	{
		LED_MODE |= (1<<9); 
	}
	
	if(setMode == 1)
	{
		LED_MODE &= ~(1<<10); 
	}
	else
	{
		LED_MODE |= (1<<10); 
	}
	
	GPIOC->ODR = LED_MODE;
	GPIOD->ODR |= (1<<2);
	GPIOD->ODR &=~(1<<2);	
}

void ShowRealData(void)//��ʾʵʱ���ݽ���
{	
	
	sprintf((char *)str, " Channel(1): %d   ", Channal_1_Frequency);
	LCD_DisplayStringLine(Line1, str);

	sprintf((char *)str, " N(1)      : %d   ", CH1_N);
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, " Channel(2): %d   ", Channal_2_Frequency);
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, " N(2)      : %d   ", CH2_N);
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "                 %d  ", selChannel+1);
	LCD_DisplayStringLine(Line9, str);
	
}

void UartAction(void)
{
	u8 i;
	
	if(RxdBuf[4] == '0')
	{
		if(RxdBuf[6]=='1' && RxdBuf[7]=='0')
		{
			CH1_N = 10;
		}
		else
		{
			CH1_N = RxdBuf[6] - '0';
		}
	}
	else if(RxdBuf[4] == '1')
	{
		if(RxdBuf[6]=='1' && RxdBuf[7]=='0')
		{
			CH2_N = 10;
		}
		else
		{
			CH2_N = RxdBuf[6] - '0';
		}
	}
	
	for(i=0; i<20; i++)//��ջ�����
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
}


void KeyAction(int code)
{
	if(code == 1)//B1
	{
		setMode ^= 1;
		Beep(100);//����ε�һ��
	}
	else if(code == 2)//B2
	{
		if(setMode == 0)//���Ա����趨ģʽ�ſ��Խ����л�ͨ��
		{
			selChannel ^= 1;//�л�����ͨ��
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 3)//B3
	{
		if(setMode == 0)//���ڱ����趨ģʽ
		{
			if(selChannel == 0)//ѡ�����ͨ��1
			{
				CH1_N--;
				if(CH1_N < 1)
				{
					CH1_N = 1;
				}
			}
			else if(selChannel == 1)
			{
				CH2_N--;
				if(CH2_N < 1)
				{
					CH2_N = 1;
				}
			}
		}
		Beep(100);//����ε�һ��
		
	}
	else if(code == 4)//B4
	{
		if(setMode == 0)//���ڱ����趨ģʽ
		{
			if(selChannel == 0)//ѡ�����ͨ��1
			{
				CH1_N++;
				if(CH1_N >= 10)
				{
					CH1_N = 10;
				}
			}
			if(selChannel == 1)
			{
				CH2_N++;
				if(CH2_N >= 10)
				{
					CH2_N = 10;
				}
			}
		}
		Beep(100);//����ε�һ��
		
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
