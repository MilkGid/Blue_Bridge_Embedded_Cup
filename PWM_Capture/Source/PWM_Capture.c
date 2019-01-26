#include "PWM_Capture.h"

u8 TIM3_CH1_CAPTURE_MODE = 0;
u32 TIM3_CH1_CAPTURE_H = 0, TIM3_CH1_CAPTURE_HL = 0;

u8 TIM3_CH2_CAPTURE_MODE = 0;
u32 TIM3_CH2_CAPTURE_H = 0, TIM3_CH2_CAPTURE_HL = 0;

u8 CAPTURE_MODE;

void TIM3_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//***ע��������APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ�� GPIOAʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM3ͨ��1-PA6 TIM3ͨ��2-PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//***���벶��ʹ��IOģʽΪ����������߸�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�������ν����ٶ��ˣ�����ʡ�ԡ�
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ�� GPIO
}

void NVIC_TIM3Enable(void)
{
    NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM3_IRQn;           //ѡ��TIM4�ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //�趨��ռ���ȼ�Ϊ0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //�趨��Ӧ���ȼ�Ϊ0
    NVIC_Init(&NVIC_initstructure);
}

void TIM3Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***ע��������APB1***
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //ʹ�ܶ�ʱ��3ʱ�� 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //***����Ϊ0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //��ʼ�� TIM3
}

void TIM3_CaptureInit(void)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM3_IOInit();//TIM3ͨ��1-PA6 TIM3ͨ��2-PA7����
	TIM3Init();//TIM3��ʱ������
	NVIC_TIM3Enable();//�ж���������

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//ѡ��ͨ��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//�����ش���
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//����ѡ��ָ��TI�Ĵ���
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//����Ƶ
	TIM_ICInitStructure.TIM_ICFilter = 0;//���˲�
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2, ENABLE);	
	TIM_Cmd(TIM3, ENABLE);
}


