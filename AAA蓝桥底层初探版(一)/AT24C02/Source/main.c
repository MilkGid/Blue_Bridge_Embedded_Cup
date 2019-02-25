/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��25��
* ��  ע��EEPROM��¼����������LCD��ʾ��������
*         
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

int main(void)
{
	u8 cnt; //������������
	u8 chk; //��������У���ֽ�
	u8 str1[25];
	
	u8 i;
	u8 str[25];
	u8 temp = 30;
	float AO = 3.845;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//��ʱ2ms
	USART2Init(9600);
	I2CInit();
	
	cnt = E2ReadByte(0x00);
	chk = E2ReadByte(0x01);
	if((cnt^chk) != 0xFF)//�����ֽڲ��Ƿ��룬��0���¼���
	{
		cnt = 0;
	}
	if(cnt < 250)
	{
		cnt ++;
	}
	LCD_ClearLine(Line8);
	sprintf((char*)str1," cnt = %d ",cnt);
	LCD_DisplayStringLine(Line8, str1);
	E2WriteByte(0x00, cnt);
	E2WriteByte(0x01, ~cnt);
	
	LCD_DisplayStringLine(Line1,(u8*) "qwertyuioplkjhgfdsazxcvb");
	sprintf((char*)str,"temp=%d   A0=%.1f  ",temp, AO);
	LCD_DisplayStringLine(Line2,str);
	
	while(1)
	{	
		KeyDriver();
		if(RxdOverFlag)
		{
			RxdOverFlag = 0;
			LCD_ClearLine(Line5);
			LCD_DisplayStringLine(Line5, RxdBuf);
			USART2_SendByte(RxdBuf);
			for(i=0; i<50; i++) RxdBuf[i] = 0;//��մ��ڽ��ջ�����
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����жϣ�������һ֡����
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
}


