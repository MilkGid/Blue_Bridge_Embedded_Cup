#include "stm32f10x.h"

void RTC_Init(u8 HH, u8 MM, u8 SS)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//RTC�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//ʹ��PWR��BKPʱ��
	PWR_BackupAccessCmd(ENABLE);//ʹ��BKP
	BKP_DeInit();//BKP��λ
	
	RCC_LSICmd(ENABLE);//ʹ��LSIʱ��
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);//�ȴ�ʱ��ʹ�����
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);//ѡ��LSI��ΪRTCʱ��Դ
	RCC_RTCCLKCmd(ENABLE);//ʹ��RTCʱ��
	
	RTC_WaitForSynchro();//�ȴ�APB1ʱ�Ӻ�RTCʱ��ͬ��
	RTC_WaitForLastTask();
	
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//ʹ�����ж�
	RTC_WaitForLastTask();
	
	RTC_SetPrescaler(40000-1);//LSIʱ��40K
	RTC_WaitForLastTask();
	
	RTC_SetCounter(HH * 3600 + MM * 60 + SS);//����RTCʱ��
	RTC_WaitForLastTask();
	
	
}
