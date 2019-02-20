#include "stm32f10x.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);//����PC��PD��
	
	//������IO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//ǿ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//8��LED��IO����
	GPIO_InitStructure.GPIO_Pin = 0xFF00;//PC8~PC15��Ӧ��LEDλ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//ǿ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOC->ODR |= 0xFF00;//��ʼ״̬Ϩ��С��
	GPIOD->ODR |= (1<<2);//ʹ��573
	GPIOD->ODR &= ~(1<<2);//ʧ��573
}
