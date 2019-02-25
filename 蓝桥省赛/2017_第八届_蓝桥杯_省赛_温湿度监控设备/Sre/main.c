/*******************************************************************************
* �ļ�����main.c
* ��  ����
* ��  �ߣ�CLAY
* �汾�ţ�v1.0.0
* ��  ��: 2019��2��20��-2019��2��22��
* ��  ע���޸ĺ��LCD����
*         2017_�ڰ˽�_���ű�_ʡ��_��ʪ�ȼ���豸
*		  ʵ���������й���
*		  ���ӣ����밴��ʱ����ķ���������
*		  		���볤�����������Ӽ�
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"
#include "pwm.h"
#include "pwm_oc.h"
#include "pwm_ic.h"
#include "timer.h"
#include "led.h"
#include "key.h"
#include "beep.h"

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);

//ȫ�ֱ���
u8 flagSet = 0;//�������ý����ʵʱ��ʾ�����л�
u8 str[20];//�ַ������õ�����
u8 flag200ms = 1;//200ms��ʱ����־

//������ر���
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;

//�������ý�����ر���
int TempThre = 40;//�¶���ֵ 40�� //ע���¶����и�ֵ��
u8 HumiThre = 80;//ʪ����ֵ 80%
u8 SampleTime= 1; //������� 1S
float TestFreq = 1.5;//�����ź�Ƶ�� 1.5KHz
//����������
int TempThre_Buf = 40;
u8 HumiThre_Buf = 80;
u8 SampleTime_Buf = 1; 
float TestFreq_Buf = 1.5;

//ʵʱ��ʾ������ر���
float Temp, Humi;
float Temp_Buf, Humi_Buf;
u8 SelIndex = 0;//���ò�������������(����ͻ����ʾ) 0-�¶� 1-ʪ�� 2-���� 3-Ƶ��

//RTC��ر���
u32 time;
u8 HH, MM, SS;
u8 HH_Buf, MM_Buf, SS_Buf;
u8 RTC_Flag = 1;

//������ر���
int Record[70][5];
u16 RecordCnt = 0;
u8 flagSampleTime = 0;

//���벶��PA1Ƶ��
extern __IO uint32_t TIM3Freq;

void RefreshTempHumi(void);//200msˢ��һ���¶Ⱥ�ʪ��
void ShowRealData(void);
void ShowSetDat(void);
void ShowLedThre(void);
void UartAction(void);
void RecordAction(void);
void ReadE2Data(void);


//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);
	PWM_IC_Init();//PA7 ���벶��
	TIM4_Init(2000, 72);//��ʱ2ms
	KeyInit();
	BeepInit();
	RTC_Init(9, 40, 0);
	ADC1_Init();
	LED_Init();//LED����
	USART2_Init();
	i2c_init();
	
	
	ReadE2Data();//��ȡE2��ԭ����ȡ�����ݣ��浽��Ӧ�Ļ�����
	
	//�ѻ����������ݸ��¸�ԭֵ
	//ע�����������Ҳ��Ҫ�ٸ�ֵ
	TempThre = TempThre_Buf;
	HumiThre = HumiThre_Buf;
	SampleTime = SampleTime_Buf;
	TestFreq = TestFreq_Buf;
	//������ӦƵ�ʵ�PWM
	PWM_OC_Init(TestFreq_Buf*1000,50);//ע������Ҫ����1000
	
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshTempHumi();//ˢ���¶Ⱥ�ʪ��
			if(!flagSet)
			{
				ShowRealData();//ʵʱ������ʾ����
			}
			else
			{
				ShowSetDat();//�������ý���
			}
			ShowLedThre();//LED1��LED3���¶Ⱥ�ʪ�ȵ���ֵ��� Ҫע�������ý����£���Ҫ�����õ���ֵʵʱ��Ч��Ҳ����˵ʵʱֵ�Ǻ�Buf��ֵ���бȽϵġ�
		}
		
		if(RxdOver)//���յ��Ժ���\n���ַ���
		{
			RxdOver = 0;
			UartAction();//ִ����Ӧ�Ĵ��ڶ���
		}
		
		if(flagSampleTime)//ÿSampleTime��¼һ��
		{
			flagSampleTime = 0;
			RecordAction();//��¼���ݵĶ���
		}
	}
}
/*���ע��������E2�е�����Ҫ�ǵ���ʱ5ms*/
void ReadE2Data(void)
{
	TempThre_Buf = E2Read(0x01);
	Delay_Ms(5);
	if(E2Read(0x00))
	{
		Delay_Ms(5);
		TempThre_Buf = -TempThre_Buf;
	}
	HumiThre_Buf = E2Read(0x02);
	Delay_Ms(5);
	SampleTime_Buf = E2Read(0x03);
	Delay_Ms(5);
	TestFreq_Buf = (float)E2Read(0x04) / 10;
	Delay_Ms(5);
}

/*���ע������дE2�е�����Ҫ�ǵ���ʱ5ms*/
void WriteE2Dta(void)
{
	//ע����E2PROM����д���ǻ�����ֵ��ԭֵ������ʵʱ��ʾ��LCD�ϵ�
	if(TempThre_Buf < 0)
	{
		E2Write(0x00, 1);//0x00�洢�¶ȵķ���-��ֵΪ1����ֵΪ0
		Delay_Ms(5);
		E2Write(0x01, (u8)(-TempThre_Buf));//0x01���¶ȵ�����ֵ
	}
	else
	{
		E2Write(0x00, 0);
		Delay_Ms(5);
		E2Write(0x01, (u8)TempThre_Buf);//0x01���¶ȵ�����ֵ
	}
	Delay_Ms(5);
	E2Write(0x02, HumiThre_Buf);//0x02��ʪ�ȵ�����ֵ
	Delay_Ms(5);
	E2Write(0x03, SampleTime_Buf);//0x03��������������ֵ
	Delay_Ms(5);
	E2Write(0x04, (u8)(TestFreq_Buf*10));//0x04�����Ƶ�ʵ�����ֵ ע��Ŵ���10��
	Delay_Ms(5);
}

void ShowLedThre(void)//LED���¶Ⱥ�ʪ�ȳ�����ֵ�����Ӧ״̬ 200msɨһ��
{
	if(Temp > TempThre_Buf)//L1 ע������Bufֵ�Ƚ�
	{
		LED_MODE ^= (1<<8);
	}
	else//��֤С�ڵ�ʱ����Ϩ��״̬
	{
		LED_MODE |= (1<<8);
	}
	
	if(Humi > HumiThre_Buf)//L2 ע������Bufֵ�Ƚ�
	{
		LED_MODE ^= (1<<9);
	}
	else//��֤С�ڵ�ʱ����Ϩ��״̬
	{
		LED_MODE |= (1<<9);
	}
	GPIOC-> ODR = LED_MODE;
	GPIOD-> ODR |= (1<<2);
	GPIOD-> ODR &=~(1<<2);
}

void RefreshTempHumi(void)//�����¶Ⱥ�ʪ��
{
	float ADC_Val;//���붨���float
	
	//�����¶�
	ADC_Val = Get_ADC() * 3.3 / 4096;
	Temp = (80 * ADC_Val / 3.3) - 20;
	
	//����ʪ��
	Humi = (80 * TIM3Freq / 1000 + 10) / 9;//ע��������裬�б�Ҫ����PC�ļ���������һ�£�����1000�����ˡ�
}

void ShowRealData(void)//��ʾʵʱ���ݽ���
{	
	LCD_SetTextColor(White);//ע������Ҫ���ϸý����µ�������ʾ��ɫ����Ȼ�ӵڶ�������
	
	LCD_DisplayStringLine(Line1, (u8 *)"     Real Data     ");
	
	sprintf((char *)str, "  Temp:  %.0fC         ", Temp);//��Ҫע�⸡����������ָ��С�����λ��
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, "  Humi:  %.0f%%        ", Humi);//ע��ٷֺ���ô�����%%
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, "  Time: %.2d-%.2d-%.2d       ", HH, MM, SS);//������ϸ�ʽ���Ʒ�.2d����õģ�
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "      RecordCnt: %d     ", RecordCnt);
	LCD_DisplayStringLine(Line9, str);
}

void ShowSetDat(void)//�������ý���
{
	LCD_SetTextColor(White);//ע������Ҫ���ϵ�0����ʾ����ɫ����Ȼѡ�����һ�е�ʱ�򣬵�0��Ҳ�����
	
	LCD_DisplayStringLine(Line1, (u8 *)"     Set Data     ");
	
	if(SelIndex == 0)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  TempThre: %dC ", TempThre);
	LCD_DisplayStringLine(Line3, str);
	
	if(SelIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  HumiThre: %d%%  ", HumiThre);
	LCD_DisplayStringLine(Line5, str);
	
	if(SelIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  SampleTime: %dS  ", SampleTime);
	LCD_DisplayStringLine(Line7, str);
	
	if(SelIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  TestFreq: %.1fKHz  ", TestFreq);
	LCD_DisplayStringLine(Line9, str);
}

void RecordAction(void)
{
	Record[RecordCnt][0] = Temp;
	Record[RecordCnt][1] = Humi;
	Record[RecordCnt][2] = HH;
	Record[RecordCnt][3] = MM;
	Record[RecordCnt][4] = SS;
	
	RecordCnt++;
	if(RecordCnt >= 65)//һ��65������
		RecordCnt = 0;
	
	LED_MODE ^= (1<<10);//LED3�����ݼ�¼��������
	GPIOC-> ODR = LED_MODE;
	GPIOD-> ODR |= (1<<2);
	GPIOD-> ODR &=~(1<<2);
}

void UartAction(void)
{
	u8 i;
	u8 UART_Str[50];
	
	if(RxdBuf[0] == 'C')
	{
		//��ȡ����ʱ�����Buf��
		HH_Buf = Record[RecordCnt-1][2];//ע����RecordCnt-1
		MM_Buf = Record[RecordCnt-1][3];
		SS_Buf = Record[RecordCnt-1][4];
		
		if(TempThre < 0)
		{		
			//���Ӹ��س����л����
			sprintf((char *)UART_Str, " TempThre:-%d,  HumiThre:%d  Time:%2d:%2d:%2d\r\n", -TempThre_Buf,HumiThre_Buf,HH_Buf,MM_Buf,SS_Buf);
		}
		else
		{
			sprintf((char *)UART_Str, " TempThre:%d,  HumiThre:%d  Time:%2d:%2d:%2d\r\n", TempThre_Buf,HumiThre_Buf,HH_Buf,MM_Buf,SS_Buf);
		}
		USART2_SendString(UART_Str);
	}
	else if(RxdBuf[0] == 'T')
	{
		for(i=0; i<RecordCnt; i++)
		{
			Temp_Buf = Record[i][0];
			Humi_Buf = Record[i][1];
			HH_Buf = Record[i][2];
			MM_Buf = Record[i][3];
			SS_Buf = Record[i][4];
			
			if(Temp_Buf < 0)
			{
				sprintf((char *)UART_Str, " Cnt:%2d Temp:-%.0f,  Humi:%.0f  Time:%2d:%2d:%2d\r\n", i, -Temp_Buf,Humi_Buf,HH_Buf,MM_Buf,SS_Buf);
			}
			else
			{
				sprintf((char *)UART_Str, " Cnt:%2d Temp:%.0f,  Humi:%.0f  Time:%2d:%2d:%2d\r\n", i, Temp_Buf,Humi_Buf,HH_Buf,MM_Buf,SS_Buf);
			}
			USART2_SendString(UART_Str);
		}
	}
	//������������
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���յ����ݴ�����Ϻ�򿪽����ж�
	for(i=0; i<20; i++)//��ջ�����
		RxdBuf[i] = 0;
}

void KeyAction(int code)
{
	if(code == 1)//����B1��LED1״̬�ı�
	{
		if(flagSet == 0)
		{
			flagSet = 1;
		}
		else if(flagSet == 1)
		{
			//��������˳�ǰ����ԭֵ��������ֵ��
			//��EEPROM����д��ʱ���û���������LCD��ʵʱ��ʾ�õ���ԭֵ
			TempThre_Buf = TempThre;
			HumiThre_Buf = HumiThre;
			SampleTime_Buf = SampleTime;
			TestFreq_Buf = TestFreq;
			//������ӦƵ�ʵ�PWM
			PWM_OC_Init(TestFreq_Buf*1000,50);//ע������Ҫ����1000
			
			WriteE2Dta();//�˳�ǰ�ѻ�����������д�뵽E2��
			
			flagSet = 0;//��־λ����
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 2)//B2
	{
		if(flagSet == 1)//�ڲ������ý����ʵ���л�����
		{
			SelIndex++;
			SelIndex &= 0x03;//����㷨�ܲ����,����
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 3)//��
	{
		if(flagSet == 1)//�ڲ������ý����ʵ���л�����
		{
			if(SelIndex == 0)
			{
				TempThre += 1;
				if(TempThre >= 60)
					TempThre = 60;
			}
			else if(SelIndex == 1)
			{
				HumiThre += 5;
				if(HumiThre >= 90)
					HumiThre = 90;
			}
			else if(SelIndex == 2)
			{
				SampleTime += 1;
				if(SampleTime >= 5)
					SampleTime = 5;
			}
			else if(SelIndex == 3)
			{
				TestFreq += 0.5;
				if(TestFreq >= 10)
					TestFreq = 10;
			}
		}
		Beep(100);//����ε�һ��
	}
	else if(code == 4)//��
	{
		if(flagSet == 1)//�ڲ������ý����ʵ���л�����
		{
			if(SelIndex == 0)
			{
				TempThre -= 1;
				if(TempThre <= -20)
					TempThre = -20;
			}
			else if(SelIndex == 1)
			{
				HumiThre -= 5;
				if(HumiThre <= 10)
					HumiThre = 10;
			}
			else if(SelIndex == 2)
			{
				SampleTime -= 1;
				if(SampleTime <= 1)
					SampleTime = 1;
			}
			else if(SelIndex == 3)
			{
				TestFreq -= 0.5;
				if(TestFreq <= 1)
					TestFreq = 1;
			}
		}
		Beep(100);//����ε�һ��
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}
