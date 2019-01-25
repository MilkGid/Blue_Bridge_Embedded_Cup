/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��24��
* ��  ע��Beep(x);��x���룻Beep(0)������ Beep(-1)��һֱ��
*         
*******************************************************************************
*/

#include "beep.h"

s32 BeepTimer = 0;

void BeepInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//����ģʽ���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BeepOff();//��ʼ���رշ�����
}

void BeepScan(s32 ms)
{
	if(BeepTimer > 0)
	{
		BeepTimer -= ms;
		if(BeepTimer <= 0)
		{
			BeepOff();
			BeepTimer = 0;
		}
	}
}

void Beep(s32 time)
{
	BeepTimer = time;
	
	if(BeepTimer == 0)
	{
		BeepOff();
	}
	else
	{
		BeepOn();
	}
}

