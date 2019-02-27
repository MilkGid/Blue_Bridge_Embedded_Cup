/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��27��
* ��  ע���޸ĺ��LCD����
*         2016_���߽�_���ű�_ʡ��_ģ��Һλ���
*		  ʵ�����й���
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
u8 flagSet = 0;//0-ʵʱ���ݽ��� 1-�������ý���
u8 str[20];//�ַ������õ�����
u8 flag200ms = 1;//200ms��ʱ����־
u8 flag1s = 0;

//������ر���
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;


//ADC���
float ADC_Val;

//��ֵ���
u8 Thre1 = 30;
u8 Thre2 = 50;
u8 Thre3 = 70;
u8 LiquidHeight = 0;//Һ��߶�
u8 LiquidLevel = 0;//Һ�������ĵȼ�
u8 LiquidLevel_Buf = 0;
u8 selcIndex = 0;// 0-����ѡ�� 1-ѡ��Thre1 2-ѡ��Thre2 3-ѡ��Thre3
u8 flagLED2 = 0;//Һ��ĵȼ��仯
u8 LED2Cnt = 0;//LED2��˸����
u8 flagLED3 = 0;//Һ��ĵȼ��仯
u8 LED3Cnt = 0;//LED3��˸����

//��������
void UartAction(void);
void ShowRealData(void);
void ShowSetData(void);
void RefreshData(void);
void LevelChange(void);


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
	
	
	//��ʼ�ϵ��E2�ж�ȡ���ݵ���ֵ������
	Thre1 = E2Read(0x00);
	Delay_Ms(5);
	Thre2 = E2Read(0x01);
	Delay_Ms(5);
	Thre3 = E2Read(0x02);
	Delay_Ms(5);
	
	
	while(1)
	{
		KeyDriver();
		
		if(flag1s)
		{
			flag1s = 0;
			LED_MODE ^= (1<<8);
			GPIOC-> ODR = LED_MODE;
			GPIOD-> ODR |= (1<<2);
			GPIOD-> ODR &=~(1<<2);
		}
				
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshData();//200msˢ��һ������
			
			if(flagSet == 0)
			{
				ShowRealData();
			}
			else if(flagSet == 1)
			{
				ShowSetData();
			}
			if(flagLED2)
			{
				LED_MODE ^= (1<<9);
				GPIOC-> ODR = LED_MODE;
				GPIOD-> ODR |= (1<<2);
				GPIOD-> ODR &=~(1<<2);
				LED2Cnt++;
				if(LED2Cnt >= 10)
				{
					LED2Cnt = 0;
					flagLED2 = 0;
				}
			}
			if(flagLED3)
			{
				LED_MODE ^= (1<<10);
				GPIOC-> ODR = LED_MODE;
				GPIOD-> ODR |= (1<<2);
				GPIOD-> ODR &=~(1<<2);
				LED3Cnt++;
				if(LED3Cnt >= 10)
				{
					LED3Cnt = 0;
					flagLED3 = 0;
				}
			}
		}
		
		if(RxdOver)//���յ��Ժ���\n���ַ���
		{
			RxdOver = 0;
			UartAction();//ִ����Ӧ�Ĵ��ڶ���
		}
		LevelChange();
	}
}

void LevelChange(void)//Һ��仯����
{
	u8 i;
	
	if(LiquidLevel_Buf != LiquidLevel)
	{
		flagLED2 = 1;
		LED2Cnt = 0;
		
		if(LiquidLevel > LiquidLevel_Buf)
		{
			sprintf((char *)str, "A:H%.2d+L%d+U\r\n", LiquidHeight, LiquidLevel);
			USART2_SendString(str);
		}
		else
		{
			sprintf((char *)str, "A:H%.2d+L%d+D\r\n", LiquidHeight, LiquidLevel);
			USART2_SendString(str);
		}
		LiquidLevel_Buf = LiquidLevel;//����Һ��߶Ȼ�����
		
		for(i=0; i<20; i++)//��ջ�����
			RxdBuf[i] = 0;	
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
	}
}

void RefreshData(void)
{
	ADC_Val = Get_ADC() * 3.3 / 4096; 
	
	LiquidHeight = ADC_Val * (100/3.3);
	
	if(LiquidHeight > Thre3)
	{
		LiquidLevel = 3;
	}
	else if(LiquidHeight > Thre2)
	{
		LiquidLevel = 2;
	}
	else if(LiquidHeight > Thre1)
	{
		LiquidLevel = 1;
	}
	else
	{
		LiquidLevel = 0;
	}
}

void ShowRealData(void)//��ʾʵʱ���ݽ���
{	
	LCD_DisplayStringLine(Line1, (u8*)"    Liquid Level    ");
	
	sprintf((char *)str, "  Height: %d         ", LiquidHeight);
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, "  ADC   : %.2f       ", ADC_Val);
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, "  Level : %d         ", LiquidLevel);
	LCD_DisplayStringLine(Line7, str);
}

void ShowSetData(void)//�������ý���
{
	LCD_SetTextColor(White);
	
	LCD_DisplayStringLine(Line1, (u8*)"  Parameter Setup   ");
	
	if(selcIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 1: %d     ", Thre1);
	LCD_DisplayStringLine(Line3, str);
	
	if(selcIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 2: %d     ", Thre2);
	LCD_DisplayStringLine(Line5, str);
	
	if(selcIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 3: %d     ", Thre3);
	LCD_DisplayStringLine(Line7, str);
	
	LCD_SetTextColor(White);
}

void UartAction(void)
{
	u8 i;

	if((RxdBuf[0]=='C') || (RxdBuf[0]=='S'))
	{
		flagLED3 = 1;
		LED3Cnt = 0;
	}
	
	if(RxdBuf[0] == 'C')
	{
		sprintf((char *)str, "C:H%.2d+L%d\r\n", LiquidHeight, LiquidLevel);
		USART2_SendString(str);
	}
	else if(RxdBuf[0] == 'S')
	{
		sprintf((char *)str, "S:TL%.2d+TM%.2d+TH%.2d\r\n", Thre1, Thre2, Thre3);
		USART2_SendString(str);
	}
	
	for(i=0; i<20; i++)//��ջ�����
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
}

void IncThre(void)
{
	if(selcIndex == 1)
	{
		Thre1++;
		if(Thre1 >= 95)
		{
			Thre1 = 95;
		}
	}
	else if(selcIndex == 2)
	{
		Thre2++;
		if(Thre2 >= 95)
		{
			Thre2 = 95;
		}
	}
	else if(selcIndex == 3)
	{
		Thre3++;
		if(Thre3 >= 95)
		{
			Thre3 = 95;
		}
	}
}

void DecThre(void)
{
	if(selcIndex == 1)
	{
		Thre1--;
		if(Thre1 <= 5)
		{
			Thre1 = 5;
		}
	}
	else if(selcIndex == 2)
	{
		Thre2--;
		if(Thre2 <= 5)
		{
			Thre2 = 5;
		}
	}
	else if(selcIndex == 3)
	{
		Thre3--;
		if(Thre3 <= 5)
		{
			Thre3 = 5;
		}
	}
}

void KeyAction(int code)
{
	if(code == 1)//B1
	{
		if(flagSet == 0)
		{
			LCD_Clear(Blue);
			
			
			selcIndex = 1;//����Thre1
			flagSet = 1;//�������ý���
		}
		else
		{
			LCD_Clear(Blue);
			
			//�˳����ý���ǰ���浱ǰ�����ݵ�������
			E2Write(0x00, Thre1);
			Delay_Ms(5);
			E2Write(0x01, Thre2);
			Delay_Ms(5);
			E2Write(0x02, Thre3);
			Delay_Ms(5);
			
			flagSet = 0;//�˳����ý���
		}
//		Beep(100);//����ε�һ��
	}
	else if(code == 2)//B2
	{
		if(flagSet == 1)
		{
			selcIndex++;
			if(selcIndex > 3)
			{
				selcIndex = 1;
			}
		}
//		Beep(100);//����ε�һ��
	}
	else if(code == 3)//B3
	{
		if(flagSet == 1)
		{
			IncThre();
		}
//		Beep(100);//����ε�һ��
		
	}
	else if(code == 4)//B4
	{
		if(flagSet == 1)
		{
			DecThre();
		}
//		Beep(100);//����ε�һ��
		
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
