#include "led.h"

void LEDInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);//����PC��PD��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//����573��������Ӧ��ʹ�ܶ�PD2
	
	GPIO_InitStructure.GPIO_Pin = 0xFF00;//ע�⿴����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ӦPC8-PC15
	
	GPIOD->ODR |= (1<<2);//PD2��1��ʹ��573������
	GPIOC->ODR |= 0xFF00;//PC8-PC15��1����ʼ״̬Ϩ��С��
	GPIOD->ODR &= ~(1<<2);//PD2��0���ر�573������
}

