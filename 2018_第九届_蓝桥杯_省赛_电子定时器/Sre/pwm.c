#include "stm32f10x.h"

/*���һ��������ʾ�����Ƿ�ʹ�����PWM*/
void PWM_Init(u32 freq, u8 duty, u8 en)
{
	u32 arr;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	arr = 1000000 / freq;//����Ƶ�ʶ�Ӧ������ֵ
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//ǿ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//72��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = arr - 1;//����ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//TIM3��ʱ��
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//����
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//����
	if(en == 1)
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}
	else
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	}
	TIM_OCInitStructure.TIM_Pulse = (arr-1) * duty / 100;//ռ�ձ�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);//TIM3_CH1
	
	TIM_Cmd(TIM3, ENABLE);//ʹ��TIM3
}

