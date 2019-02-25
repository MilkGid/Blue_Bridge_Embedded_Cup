#include "adc.h"

void ADC1_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PB��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ѡ��PB0����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��IO
}

void ADC_Cali(void)
{
	ADC_ResetCalibration(ADC1);//ʹ�ܸ�λУ׼
	while(ADC_GetResetCalibrationStatus( ADC1));//�ȴ���λУ׼����
	ADC_StartCalibration( ADC1);//����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼����
}

void ADC1Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC1_IOInit();//ADC��GPIO����
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ʹ��ADCʱ��,APB2����
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC���ʱ�Ӳ�����14M�����ﾡ���˶�APB2ʱ�ӽ���6��Ƶ=12M
	ADC_DeInit(ADC1);//��λADC
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//ͨ��ģʽѡ��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//ת��ģʽѡ�񵥴�ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�����鴥��ת����ʽѡ��ת���������������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//���ݸ�ʽѡ���Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//ͨ����ĿΪ1
	ADC_Init(ADC1,&ADC_InitStructure);//��ʼ���ṹ��
	
	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC
	ADC_Cali();//ADCУ׼
}

u16 Get_ADC(u8 channel)
{
	u16 temp;
	
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);//ADC����ʱ��239.5���� ��ת��ʱ��=����ʱ��+12.5����
	ADC_SoftwareStartConvCmd( ADC1,ENABLE);//ʹ��ADC1�������ת��������һ��ADCת��
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);//�ȴ�ADCת�����
	temp = ADC_GetConversionValue( ADC1);//��ȡת�������ע���ʱ�Ѿ��Զ����EOCλ
	ADC_SoftwareStartConvCmd( ADC1,DISABLE);//ʧ��ADCת��
	
	return temp;
}

