/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��25��
* ��  ע���޸ĺ��LCD����
*         2016_���߽�_���ű�_ʡ��_��ѹ��������豸
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

//������ر���
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;
u8 LED_Sta = 1;//LED������0-�أ� 1-�� Ĭ���ǿ�

//RTC��ر���
u8 RTC_Flag = 0;
u32 time = 0;
u8 HH = 23, MM = 59, SS = 55;//ʵʱʱ��
u32 time_Buf = 0;
u8 HH_Buf = 0, MM_Buf = 0, SS_Buf = 0;//�ϱ�ʱ�� 
u8 timeIndex = 0;//0-˭�������� 1-HH���� 2-MM���� 3-SS����

//ADC���
float ADC_Val;
float k_Val = 0;

//��������
void UartAction(void);
void Refresh(void);
void ShowRealData(void);
void ShowSetData(void);
u32 TimeToSecs(u8 hour, u8 min, u8 sec);
void SubmitVolt(void);

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
	RTC_Init(HH, MM, SS);
	ADC1_Init();
	LED_Init();//LED����
	USART2_Init();
	i2c_init();
		
	k_Val = E2Read(0x00) / 10.0;
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Refresh();
			
			if(flagSet == 0)
			{
				ShowRealData();
			}
			else if(flagSet == 1)
			{
				ShowSetData();
			}
		}
		
		if(RxdOver)//���յ��Ժ���\n���ַ���
		{
			RxdOver = 0;
			UartAction();//ִ����Ӧ�Ĵ��ڶ���
		}
		
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			SubmitVolt();
		}
	}
}

void SubmitVolt(void)
{	
	if(time == (HH_Buf*3600 + MM_Buf*60 + SS_Buf))
	{
		sprintf((char *)str, "%.2f+%.1f+%.2d%.2d%.2d\n", ADC_Val, k_Val, HH_Buf, MM_Buf, SS_Buf);
		USART2_SendString(str);
	}
}

void ShowLedThre(void)
{
	if(LED_Sta && (ADC_Val > (3.3*k_Val/10) ) )
	{
		LED_MODE ^= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
	else
	{
		LED_MODE |= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
}

void Refresh(void)//ʵʱ����ˢ��
{
	ADC_Val = Get_ADC() * 3.3 / 4096;
	ShowLedThre();//ˢ��LED״̬
}

void ShowRealData(void)//��ʾʵʱ���ݽ���
{	
	sprintf((char *)str, "  V1: %.2f         ", ADC_Val);
	LCD_DisplayStringLine(Line1, str);
	
	sprintf((char *)str, "  k: %.1f          ", k_Val);
	LCD_DisplayStringLine(Line3, str);
	
	if(LED_Sta)
	{
		LCD_DisplayStringLine(Line5, (u8*)"  LED: ON          ");
	}
	else
	{
		LCD_DisplayStringLine(Line5, (u8*)"  LED: OFF          ");
	}
	
	sprintf((char *)str, "  T: %.2d-%.2d-%.2d       ", HH, MM, SS);
	LCD_DisplayStringLine(Line7, str);
	
	
	LCD_DisplayStringLine(Line9, (u8*)"                1 ");
}

void RefreshTime(void)
{
	if(timeIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 240, HH_Buf/10 + '0');//��Ӧ������ת��ΪASCII
	LCD_DisplayChar(Line6, 225, HH_Buf%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 210, '-');
	
	if(timeIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 195, MM_Buf/10 + '0');
	LCD_DisplayChar(Line6, 180, MM_Buf%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 165, '-');
	
	if(timeIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 150, SS_Buf/10 + '0');
	LCD_DisplayChar(Line6, 135, SS_Buf%10 + '0');
	
	LCD_SetTextColor(White);	
}

void ShowSetData(void)//�������ý���
{
	LCD_DisplayStringLine(Line3, (u8*)"     Setting          ");
	
	RefreshTime();
	
	LCD_DisplayStringLine(Line9, (u8*)"                 2 ");
}

void UartAction(void)
{
	u8 i;

	if(RxdBuf[0] == 'k')
	{
		if( ( (RxdBuf[3]-'0') >= 1) && ( (RxdBuf[3]-'0') <= 9) )
		{
			k_Val = (RxdBuf[3] - '0') / 10.0;//ע���������10.0����Ȼ�ó��Ľ���ǽضϺ��
			E2Write(0x00, RxdBuf[3] - '0');
			USART2_SendString((u8*)"OK\n");
		}
	}
	
	for(i=0; i<20; i++)//��ջ�����
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
}

void IncTime()
{
	if(timeIndex == 1)
	{
		HH_Buf += 1;
		if(HH_Buf >= 60)
		{
			HH_Buf = 0;
		}
	}
	else if(timeIndex == 2)
	{
		MM_Buf += 1;
		if(MM_Buf >= 60)
		{
			MM_Buf = 0;
		}
	}
	else if(timeIndex == 3)
	{
		SS_Buf += 1;
		if(SS_Buf >= 60)
		{
			SS_Buf = 0;
		}
	}
}

void KeyAction(int code)
{
	if(code == 1)//B1
	{
		LED_Sta ^= 1;
		Beep(100);//����ε�һ��
	}
	else if(code == 2)//B2
	{
		if(flagSet == 0)
		{
			flagSet = 1;
			
			LCD_Clear(Blue);
			HH_Buf = HH;
			MM_Buf = MM;
			SS_Buf = SS;
			
			timeIndex = 1;
		}
		else if(flagSet == 1)
		{
			flagSet = 0;
			
			LCD_Clear(Blue);
			
			timeIndex = 0;
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 3)//B3
	{
		if(flagSet)
		{
			timeIndex++;
			if(timeIndex > 3)
			{
				timeIndex = 1;
			}
			Beep(100);//����ε�һ��
		}
		
	}
	else if(code == 4)//B4
	{
		if(flagSet)
		{
			IncTime();
			Beep(100);//����ε�һ��
		}
		
	}
	
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
