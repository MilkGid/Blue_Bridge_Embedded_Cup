#include "timer.h"


void NVIC_TIM2Enable(void)
{
	NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM2_IRQn;           //ѡ��TIM2�ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //�趨��ռ���ȼ�Ϊ0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //�趨��Ӧ���ȼ�Ϊ0
    NVIC_Init(&NVIC_initstructure);

}

void TIM2Init(u16 period, u16 prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//ע��TIM2��APB1������
	TIM_TimeBaseStructure.TIM_Period = period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	NVIC_TIM2Enable();
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//�Ͻ������������һ��0
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//ʹ���ж�
	TIM_Cmd(TIM2, ENABLE);//ʹ�ܶ�ʱ��
}
	

