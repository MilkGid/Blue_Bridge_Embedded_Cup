#include "PWM_Compare.h"

void TIM4_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	//***ע��������APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ�� GPIOBʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM4ͨ��1-PB6 TIM4ͨ��2-PB7
	//***�����������(PWMģʽ)***
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //��ʼ�� GPIO
}

void NVIC_TIM4Enable(void)
{
    NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM4_IRQn;           //ѡ��TIM4�ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //�趨��ռ���ȼ�Ϊ0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //�趨��Ӧ���ȼ�Ϊ0
    NVIC_Init(&NVIC_initstructure);
}

void TIM4Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***ע��������APB1***
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //ʹ�ܶ�ʱ��4ʱ�� 
	//***ע���������õ���0xFFFF
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //���ü���ֵ���
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //��ʼ�� TIM4
	//***�������������***
	NVIC_TIM4Enable();
}

u32 CH1_Val,  CH2_Val;
u32  CH1_Duty,  CH2_Duty;

void TIM4_PWMCompare(u32 ch1_freq, u32 ch2_freq, u32 ch1_duty, u32 ch2_duty)
{
//	u32 CH1_Val,  CH2_Val;//FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	TIM_OCInitTypeDef TIM_OCInitStructure;

	CH1_Val = 1000000 / ch1_freq;
	CH2_Val = 1000000 / ch2_freq;
	CH1_Duty = CH1_Val * ch1_duty / 100;
	CH2_Duty = CH2_Val * ch2_duty / 100;
	
	TIM4_IOInit();//TIM4ͨ��1-PB6 TIM4ͨ��2-PB7����
	TIM4Init();//TIM4��ʱ������
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//����ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//������Ե�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CH1_Val;
	//TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//Ԥװ��ʹ��λ
	TIM_OC1Init( TIM4, &TIM_OCInitStructure);
			
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//����ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//������Ե�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_Pulse = CH2_Val;
	//TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//Ԥװ��ʹ��λ;
	TIM_OC2Init( TIM4, &TIM_OCInitStructure);
	
	TIM_SetCounter(TIM4, 0);//��ʱ������ֵ��0
	TIM_SetCompare1(TIM4, 0);//��ʱ������Ƚ�1�Ĵ���ֵ��0
	TIM_SetCompare2(TIM4, 0);//��ʱ������Ƚ�2�Ĵ���ֵ��0
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_CC2);//����жϱ�־λ //���Ҳ��Ӱ�켫�ԣ�
	TIM_ITConfig(TIM4, TIM_IT_CC1|TIM_IT_CC2, ENABLE);//ʹ��ͨ���Ƚ��ж�
	TIM_Cmd(TIM4, ENABLE); //ʹ�� TIM4
}



