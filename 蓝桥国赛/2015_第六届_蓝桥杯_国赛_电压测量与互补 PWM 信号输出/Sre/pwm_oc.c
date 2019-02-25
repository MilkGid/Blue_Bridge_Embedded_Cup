#include "stm32f10x.h"

/*sta������ʾ�Ƿ��ʼ��IO��en������ʾ�Ƿ�ʹ��ͨ�����*/
void PWM_OC_Init(u8 freq, u8 duty, u8 sta, u8 en)
{
	u16 freq_Buf;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//�������ȿ�����Ӧʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1, ENABLE);
	
	if(sta)//�������������Ҫ��ε��ã����ǲ�����Ҫÿ�ζ����³�ʼ��IO����������������
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//ǿ�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//ǿ�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	
	freq_Buf = 1000000 / (freq*1000);//ע������Ҫ���зŴ�
	
	TIM_TimeBaseStructure.TIM_Period = freq_Buf - 1;//�ܵ�ֵ����Ϊ0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//ת��ģʽ
	if(en)
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	}
	else
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	}
	TIM_OCInitStructure.TIM_Pulse = (freq_Buf - 1) * duty / 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//�߼���ʱ���������
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);//ע��������OC2
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//�߼���ʱ�����������
	TIM_Cmd(TIM1, ENABLE);//������ʱ��
	
}


