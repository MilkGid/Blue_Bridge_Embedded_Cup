/*******************************************************************************
* �ļ�����usart.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��1��25��
* ��  ע���򵥵Ĵ���ͨ�ţ����Ͳ�����������ѯ�ķ�ʽ�����մӲ����жϷ�ʽ
*         ���ջ��������ǵ�һ��Ҫ��ʾ����ʾ����Ե�ʣ����������˸���20��
*******************************************************************************
*/

#include "usart.h"

u8 RxdBuf[50];//���ջ�����
u8 RxdOverFlag = 0;//������ɱ�־
u8 RxdCnt = 0;//���ձ�������

void USART2_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//PA��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2 - TXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3 - RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_USART2Enable(void)
{
	NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = USART2_IRQn;           //ѡ��TIM2�ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //ʹ���ж�ͨ��
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //�趨��ռ���ȼ�Ϊ0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //�趨��Ӧ���ȼ�Ϊ0
    NVIC_Init(&NVIC_initstructure);
}

void USART2Init(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART2_IOInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStructure.USART_BaudRate = baud; //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; //��У��λ
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); //��ʼ��USART2
	NVIC_USART2Enable();//�����ж����ȼ�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2,ENABLE);//ʹ��USART2
}

void USART2_SendByte(u8 *str)
{
	u8 index = 0;
	
	while(str[index] != 0)
	{
		USART_SendData(USART2, str[index]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);//=1��ʾ�������
		index++;
	}
}

void USART2_RecByte(void)
{
	u8 tmp;
	tmp =  USART_ReceiveData(USART2);
	if(tmp != '\n')//��β��־����'\n'����һ�η��͵����ݲ��ܳ���20(Һ��һ����ʾ20��)
	{
		RxdBuf[RxdCnt++] = tmp;
		if(RxdCnt >= 20)
			RxdCnt = 0;
	}
	else
	{
		RxdCnt = 0;
		RxdOverFlag = 1;
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//�رս����жϣ��ȴ���������ٴ�
	}
	
}



