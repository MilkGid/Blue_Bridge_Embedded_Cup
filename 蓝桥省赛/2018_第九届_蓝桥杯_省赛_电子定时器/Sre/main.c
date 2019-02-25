/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��24��
* ��  ע���޸ĺ��LCD����
*         2018_�ھŽ�_���ű�_ʡ��_���Ӷ�ʱ��
*		  ʵ�����й���
*		  �������������з�����100ms������
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
u8 flag500ms = 0;
u8 flag200ms = 0;//200ms��ʱ����־
u8 flag1s = 0; //������ʱ��ʱ��ĵ���ʱ
u8 flagMode = 0;//����ģʽ 0-ֹͣģʽ(Standby) 1-����ģʽ(Setting) 2-����ģʽ(Running) 3-��ͣ(Pause)
u8 str[20];//�ַ������õ�����
u8 saveIndex = 0; //�洢�������� ��5������(0-4)
u8 timeIndex = 0; //����ĳһλ���� 0-û�и��� 1-ʱ 2-�� 3-��

//LED
u16 LED_MODE = 0XFFFF;

//����ʱ��ر���
int secs=0;//ʱ�任��ɵ�����
u8 hour=0, min=2, sec=1;//ʱ���Ӧ��ʱ����

//��������
void Show(void);
void ReadE2Time(void);
void TimeToSecs(void);
void SecsToTime(void);


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
	LED_Init();//LED����
	i2c_init();//E2PROM�����ȳ�ʼ��I2C
	
	//��ʼ������ʱ����
	ReadE2Time();//��ȡE2�д洢��λ��0��ʱ�����
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Show();
		}
		if(flag1s)
		{
			flag1s = 0;
			if(flagMode == 2)//��������ģʽ
			{
				TimeToSecs();
				secs--;
				if(secs <= 0)
					secs = 0;
				SecsToTime();
			}
		}
	}
}

/*��ʱ��ת���������ʽ�����㵹��ʱ*/
void TimeToSecs(void)
{
	secs = hour*3600 + min*60 + sec;
}

/*����ת����ʱ�����ʽ��������ʾ�ʹ洢*/
void SecsToTime(void)
{
	hour = secs / 3600;
	min  = (secs % 3600) / 60;
	sec  = (secs % 3600) % 60;
}

void RefreshTime(void)
{
	if(timeIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 240, hour/10 + '0');//��Ӧ������ת��ΪASCII
	LCD_DisplayChar(Line4, 225, hour%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 210, ':');
	
	if(timeIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 195, min/10 + '0');
	LCD_DisplayChar(Line4, 180, min%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 165, ':');
	
	if(timeIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 150, sec/10 + '0');
	LCD_DisplayChar(Line4, 135, sec%10 + '0');
	
	LCD_SetTextColor(White);
	
}

void Show(void)
{
	sprintf((char*)str, "  No %d       ", saveIndex);
	LCD_DisplayStringLine(Line1, str);
	
	RefreshTime();
	
	switch(flagMode)
	{
		case 0: LCD_DisplayStringLine(Line7, (u8*)"     Standby     "); break;
		case 1: LCD_DisplayStringLine(Line7, (u8*)"     Setting     "); break;
		case 2: LCD_DisplayStringLine(Line7, (u8*)"     Running     "); break;
		case 3: LCD_DisplayStringLine(Line7, (u8*)"     Pause       "); break;
		default: break;
	}
	
	if(flagMode == 2)//��������ģʽ���PWM����˸LED1
	{
		/*PA6���1K 80%��PWM�ź�*/
		PWM_Init(1000, 80, 1); //����ʹ��PWM��
		
		if(flag500ms == 1)
		{
			flag500ms = 0;
			
			LED_MODE ^= (1<<8);
			GPIOC-> ODR = LED_MODE;
			GPIOD-> ODR |= (1<<2);
			GPIOD-> ODR &=~(1<<2);
		}
	}
	else
	{
		/*PA6���1K 80%��PWM�ź�*/
		PWM_Init(1000, 80, 0);//��ֹʹ��PWM��
		LED_MODE |= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
}


void ReadE2Time()
{
	switch(saveIndex)
	{
		case 0: hour = E2Read(0x00); Delay_Ms(5);
				min  = E2Read(0x01); Delay_Ms(5);
				sec  = E2Read(0x02); Delay_Ms(5);
				break;
		case 1: hour = E2Read(0x03); Delay_Ms(5);
			    min  = E2Read(0x04); Delay_Ms(5);
		        sec  = E2Read(0x05); Delay_Ms(5);
		        break;
		case 2: hour = E2Read(0x06); Delay_Ms(5);
			    min  = E2Read(0x07); Delay_Ms(5);
		        sec  = E2Read(0x08); Delay_Ms(5);
		        break;
		case 3: hour = E2Read(0x09); Delay_Ms(5);
			    min  = E2Read(0x0A); Delay_Ms(5);
		        sec  = E2Read(0x0B); Delay_Ms(5);
		        break;
		case 4: hour = E2Read(0x0C); Delay_Ms(5);
			    min  = E2Read(0x0D); Delay_Ms(5);
		        sec  = E2Read(0x0E); Delay_Ms(5);
		        break;
		default: break;
	}
}

void WriteE2Time()
{
	switch(saveIndex)
	{
		case 0: E2Write(0x00, hour); Delay_Ms(5);
				E2Write(0x01, min ); Delay_Ms(5);
				E2Write(0x02, sec ); Delay_Ms(5);
				break; 
		case 1: E2Write(0x03, hour); Delay_Ms(5);
			    E2Write(0x04, min ); Delay_Ms(5);
		        E2Write(0x05, sec ); Delay_Ms(5);
		        break; 
		case 2: E2Write(0x06, hour); Delay_Ms(5);
			    E2Write(0x07, min ); Delay_Ms(5);
		        E2Write(0x08, sec ); Delay_Ms(5);
		        break; 
		case 3: E2Write(0x09, hour); Delay_Ms(5);
			    E2Write(0x0A, min ); Delay_Ms(5);
		        E2Write(0x0B, sec ); Delay_Ms(5);
		        break; 
		case 4: E2Write(0x0C, hour); Delay_Ms(5);
			    E2Write(0x0D, min ); Delay_Ms(5);
		        E2Write(0x0E, sec ); Delay_Ms(5);
		        break;
		default: break;
	}
}

void IncTime()
{
	if(timeIndex == 1)
	{
		hour += 1;
		if(hour >= 60)
		{
			hour = 0;
		}
	}
	else if(timeIndex == 2)
	{
		min += 1;
		if(min >= 60)
		{
			min = 0;
		}
	}
	else if(timeIndex == 3)
	{
		sec += 1;
		if(sec >= 60)
		{
			sec = 0;
		}
	}
}

void KeyAction(int code, u8 sta)
{
	/*�̰���Ӧ�Ķ���*/
	if(sta == 0)
	{
		if(code == 1)//����B1
		{
			if(flagMode == 0)//����ֹͣģʽ�����л��洢λ��
			{
				saveIndex++;
				if(saveIndex > 5)
				{
					saveIndex = 0;
				}
				ReadE2Time();
			}
		}
		else if(code == 2)//B2
		{
			if(flagMode == 0)//��ֹͣģʽ
			{
				flagMode = 1;//���º��л�������ģʽ
				timeIndex = 1;//��ʱ����������ŵ�"ʱ"��
			}
			else if(flagMode == 1)//������ģʽ
			{
				timeIndex++;
				if(timeIndex > 3)
				{
					timeIndex = 1;
				}
			}
			
		}
		else if(code == 3)//B3
		{
			if(flagMode == 1)
			{
				IncTime();
			}
		}
		else if(code == 4)//B4
		{
			if(flagMode == 0)//����ֹͣģʽ�̰�,����������ʱ��
			{
				flagMode = 2;
			}
			else if(flagMode == 1)//��������ģʽ�������������洢����ֱ������
			{
				flagMode = 2;//��������ģʽ
				timeIndex = 0;//ȥ�����и���
			}
			else if(flagMode == 2)//�����ڼ�̰�
			{
				flagMode = 3;//������ͣģʽ
			}
			else if(flagMode == 3)//������ͣģʽ�̰����ָ���ʱ������ʱ
			{
				flagMode = 2;
			}
		}
	}
	/*������Ӧ�Ķ���*/
	else if(sta == 1)
	{
		if(code == 2)//����B2
		{
			if(flagMode == 1)//��������ģʽ
			{
				WriteE2Time();//�˳�ǰ�������õ�ʱ�����ݵ�E2��
				flagMode = 0;//����ֹͣģʽ
				timeIndex = 0;//ȥ�����и���
				Beep(100);//����ε�һ��
			}
		}
		else if(code == 3)//B3
		{
			if(flagMode == 1)
			{
				IncTime();
				Beep(100);//����ε�һ��
			}
		}
		else if(code == 4)//B4
		{
			if((flagMode==2) || (flagMode==3))//�������л�����ͣģʽ���������ֹͣģʽ
			{
				flagMode = 0;
				Beep(100);//����ε�һ��
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
