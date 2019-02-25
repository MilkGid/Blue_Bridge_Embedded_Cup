#include "PWMMode_Advance.h"

void TIM1_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//***ע��������APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //ʹ�� GPIOAʱ��
	//***����ʵ�ʵ�·ѡ����USART1�ĸ��ù��ܡ�***(Ҫע����ǣ��ε�����ñPA9 PA10)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10; //TIM_CH2 - PA9 //TIM_CH3 - PA10
	//***�����������(PWMģʽ)***
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ�� GPIO
}

void TIM1Init(u32 arr, u8 ch1_duty, u8 ch2_duty)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***ע��������APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //ʹ�ܶ�ʱ��1ʱ�� 
	TIM_TimeBaseStructure.TIM_Period = arr-1; //�������Զ���װ������ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //��ʼ�� TIM1
}

void TIM1_PWMInit(u32 freq, u8 ch1_duty, u8 ch2_duty)
{
	u32 arr;
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	arr = 1000000/ freq;
	
	TIM1_IOInit();//TIM1ͨ��2-PA9 TIM1ͨ��3-PA10����
	TIM1Init(arr, ch1_duty, ch2_duty);//TIM1��ʱ������
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM1ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//������Ը�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (arr - 1) * ch1_duty / 100;
	//TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);//Ԥװ��ʹ��λ����ν����Ӱ��
	TIM_OC2Init( TIM1, &TIM_OCInitStructure);
			
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//PWM2ģʽ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//������Ե�
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	//***(arr-1)���ܵļ���ֵ����(arr-1)*ռ�ձ��ǵ�ƽ��תʱ�ļ���ֵ���ʿ���pluse�Ϳ��Կ���ռ�ձ�***
	TIM_OCInitStructure.TIM_Pulse = (arr - 1) * ch2_duty / 100;
	//TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);//Ԥװ��ʹ��λ����ν����Ӱ��
	TIM_OC3Init( TIM1, &TIM_OCInitStructure);
	
	//***�߼���ʱ����������һ�䣬��ͨ�þͲ�����***
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//ʹ��PWM��� 
	TIM_Cmd(TIM1, ENABLE); //ʹ�� TIM1
}

