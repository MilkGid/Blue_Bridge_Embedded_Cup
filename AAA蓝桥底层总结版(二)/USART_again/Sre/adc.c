#include "stm32f10x.h"

void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);//����GPIOʱ�Ӻ�ADC1ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADCʱ�����14M�����ٽ���6��Ƶ!!!
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//ѡ��PB0����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//�������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;//ͨ����Ŀ1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC
	
	//ADCУ׼(У׼ǰ������ʹ��ADC)
	ADC_ResetCalibration(ADC1);//ʹ�ܸ�λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);//����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�У׼����
}

u16 Get_ADC(void)
{
	u16 tmp;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);//ת��ʱ��239.5������
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//�������
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);//�ȴ�ת�����
	tmp = ADC_GetConversionValue(ADC1);//��ȡת�����(��ȡת��������Զ���0-EOC��־λ)
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);//������һ�����ת��
	
	return tmp;
}

