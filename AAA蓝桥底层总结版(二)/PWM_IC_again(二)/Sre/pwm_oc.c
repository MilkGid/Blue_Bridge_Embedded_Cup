#include "stm32f10x.h"

u32 CH2_Val;
u32 CH2_Duty;//����Ŷ�������ע��������ռ�ձȶ�Ӧ�ļ���ֵ������С��100����������

void PWM_OC_Init(u32 freq, u8 duty)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	CH2_Val = 1000000 / freq;//Ƶ�ʶ�Ӧ�ļ���ֵ
	CH2_Duty = CH2_Val * duty / 100;//ռ�ձȶ�Ӧ�ļ���ֵ
	
	//�������ȿ�����Ӧʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//ǿ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//����Ƚ���Ҫ�õ���ʱ���жϣ������������ж�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;//�ܵ�ֵ����Ϊ0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//ת��ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CH2_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);//ע��������OC2
	
	TIM_SetCounter(TIM2, 0);//��������0
	TIM_SetCompare2(TIM2, 0);//���ñȽ�ֵ
	
	TIM_Cmd(TIM2, ENABLE);//������ʱ��
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);//������ʱ���ж�
	
}


