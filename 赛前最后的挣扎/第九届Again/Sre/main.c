/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��3��23��
* ��  ע���ھŽ���������
*         
*		 
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "i2c.h"
#include "e2prom.h"
#include "stdio.h"
#include "pwm.h"


//ȫ�ֱ���
u32 TimingDelay = 0;
u8 str[20];
u8 flag200ms = 0;
u8 flag1s = 0;
u8 flag500ms = 0;


//ADC���
float ADC_VAL = 0;


//RTC���
u8 HH, MM, SS;
u8 RTC_FLAG;
u32 time;


//USART2���
u8 RxdOver;
u8 RxdCnt;
u8 RxdBuf[20];

//�洢״̬���
u8 saveIndex = 0;//5���洢λ��(0-4)
u8 hour=1, min=4, sec=2; //�����5����ʱʱ��,�������ʵʱ��ʾ
u8 hourBuf=1, minBuf=2, secBuf=3;//��������洢
u8 timeIndex = 0;//ʱ������������ 1-ʱ 2-�� 3-�� 0-��������
u8 saveSta = 0;//���Ӷ�ʱ����4�ֹ���״̬ 0-ֹͣStandby 1-����Setting 2-����Running 3-��ͣPause


//��������
void Delay_Ms(u32 nTime);
void Show(void);
void ReadTimeData(void);
void RefreshTime(void);
void RefreshLed(u8 sta);


//Main Body
int main(void)
{
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);


	//��ʼ������
	LED_Init();
	TIM4_Init(2000, 72);
	KEY_Init();
	i2c_init();
	PWM_Init(0);

	ReadTimeData();//��ȡE2�еĶ�ʱʱ��
	hour = hourBuf;
	min = minBuf;
	sec = secBuf;

	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Show();
			
			if(saveSta == 2)//��������״̬
			{
				PWM_Init(1);//���PWM
				RefreshLed(1);//0.5s��˸LED
			}
			else
			{
				PWM_Init(0);//ֹͣ���PWM
				RefreshLed(0);//�ر�LED
			}
			
		}
		if(flag1s)
		{
			flag1s = 0;
			if(saveSta == 2)
			{
				RefreshTime();
			}
		}
	}
}

void RefreshLed(u8 sta)
{
	static u8 ledSta = 0;
	
	if(sta)
	{
		if(flag500ms)
		{
			flag500ms = 0;
			
			if(ledSta)
			{
				GPIOD->ODR |= (1<<2);
				GPIOC->ODR = 0xFF00;
				GPIOD->ODR &=~(1<<2);
				ledSta = 0;
			}
			else
			{
				GPIOD->ODR |= (1<<2);
				GPIOC->ODR = 0xFE00;
				GPIOD->ODR &=~(1<<2);
				ledSta = 1;
			}
		}
	}
	else
	{
		GPIOD->ODR |= (1<<2);
		GPIOC->ODR = 0xFF00;
		GPIOD->ODR &=~(1<<2);
	}
}

void RefreshTime(void)
{
	int time;
	
	time = hour * 3600 + min * 60 + sec;
	
	time--;
	if(time == 0)
	{
		time = 0;
	}
	
	hour = time / 3600;
	min  = time % 3600 / 60;
	sec  = time % 3600 % 60;
}

void Show(void)
{
	//����λ��
	sprintf((char *)str, "  No %d   ", saveIndex);
	LCD_DisplayStringLine(Line2, str);
	
	//��ʱ��ʱ������
	if(timeIndex == 1)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}	
	LCD_DisplayChar(Line5, 250, hour/10+'0');
	LCD_DisplayChar(Line5, 235, hour%10+'0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line5, 220, ':');
	
	
	if(timeIndex == 2)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}
	LCD_DisplayChar(Line5, 195, min/10+'0');
	LCD_DisplayChar(Line5, 180, min%10+'0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line5, 165, ':');
	
	if(timeIndex == 3)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}
	LCD_DisplayChar(Line5, 140, sec/10+'0');
	LCD_DisplayChar(Line5, 125, sec%10+'0');
	
	LCD_SetTextColor(White);
	
	
	//ϵͳ״̬
	if(saveSta == 0)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Standby  ");
	}
	else if(saveSta == 1)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Setting  ");
	}
	else if(saveSta == 2)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Running  ");
	}
	else if(saveSta == 3)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Pause  ");
	}
}

//����ʱ�䵽E2��
void WriteTimeData(void)
{
	E2Write(saveIndex * 3, hourBuf);
	Delay_Ms(5);
	E2Write(saveIndex * 3 + 1, minBuf);
	Delay_Ms(5);
	E2Write(saveIndex * 3 + 2, secBuf);
	Delay_Ms(5);
}

void ReadTimeData(void)
{
	hourBuf = E2Read(saveIndex * 3);
	Delay_Ms(5);
	minBuf = E2Read(saveIndex * 3 + 1);
	Delay_Ms(5);
	secBuf = E2Read(saveIndex * 3 + 2);
	Delay_Ms(5);
}

void IncTime(void)
{
	if(timeIndex == 1)
	{
		hour++;
		if(hour >= 24)
		{
			hour = 0;
		}
	}
	else if(timeIndex == 2)
	{
		min++;
		if(min >= 59)
		{
			min = 0;
		}
	}
	else if(timeIndex == 3)
	{
		sec++;
		if(sec >= 59)
		{
			sec = 0;
		}
	}
}

//staΪ0��ʾ�̰� 1-��ʾ����
void KeyAction(u8 code, u8 sta)
{
	if(sta == 0)//�̰�
	{
		if(code == 1)
		{
			if(saveSta == 0)//��ֹͣģʽ���л�����
			{
				saveIndex++;
				if(saveIndex > 4)
				{
					saveIndex = 0;
				}
				
				ReadTimeData();
				hour = hourBuf;
				min = minBuf;
				sec = secBuf;
				
			}
			
		}
		else if(code == 2)//�̰�B2��������״̬
		{
			saveSta = 1;//��������״̬
			timeIndex++;//����ʱ->��->���˳��
			if(timeIndex > 3)
			{
				timeIndex = 1;
			}
			
		}
		else if(code == 3)
		{
			if(saveSta == 1)//��������״̬
			{
				IncTime();
			}
		}
		else if(code == 4)
		{
			if(saveSta == 0)//����ֹͣģʽ��
			{
				saveSta = 2;//����Ϊ����ģʽ
			}
			else if(saveSta == 2)//��������ģʽ��
			{
				saveSta = 3;//��ͣ��ʱ��
			}
			else if(saveSta == 3)//������ͣģʽ��
			{
				saveSta = 2;//�ָ�����ģʽ
			}
			else if(saveSta == 1)//��������ģʽ��(��ʱ����ʱ��)
			{
				saveSta = 2;//��ʱ������Ϻ�ֱ���л�������ģʽ�����ǲ����浽E2��
				timeIndex = 0;//ȡ������
			}
		}
	}
	else if(sta == 1)//����
	{
		if(code == 2)
		{
			hourBuf = hour;//�˳�����ģʽǰ�����²���
			minBuf = min;
			secBuf = sec;
			
			saveSta = 0;//����ֹͣģʽ
			timeIndex = 0;//ȥ������
			
			WriteTimeData();//д������ʱ�䵽E2��
		}
		else if(code == 3)
		{
			if(saveSta == 1)//��������ģʽ��
			{
				IncTime();
			}
		}
		else if(code == 4)
		{
			if((saveSta==2) || (saveSta==3))//��������ģʽ����ͣģʽ
			{
				saveSta = 0;//��Ϊֹͣģʽ
			}
		}
	}
}	

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
