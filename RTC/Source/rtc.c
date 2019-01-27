#include "rtc.h"

void NVIC_RTCEnable(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  //ʹ��RTC�ж�
}

void RTCInit(u8 HH,u8 MM,u8 SS)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();
	
	NVIC_RTCEnable();
	
	RCC_LSICmd(ENABLE); //ʹ��LSIʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); //ѡ��LSI��Ϊʱ��Դ 
	RCC_RTCCLKCmd(ENABLE); //ʹ��RTCʱ��
	
	RTC_WaitForSynchro();//APB1ʱ����RTCʱ��ͬ��
	RTC_WaitForLastTask();//�ȴ��������
	
	//���¶�RTC�Ĵ�������
	RTC_SetPrescaler(40000 - 1);  //���÷�Ƶ
	RTC_WaitForLastTask(); //�ȴ��������
	
	RTC_SetCounter(3600 * HH + 60 * MM + SS); //���ó�ʼʱ��
	RTC_WaitForLastTask();
	
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//���ж�ʹ��
	RTC_WaitForLastTask();
}

