#include "config.h"

/* 1/4΢����ʱ����(���������ü�����ʱ�乲�ƺ�ʱԼ1/4΢��@72MHz��Ƶ) */
void delay_qus(void)
{
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
	__ASM ("nop");
}

/* ΢����ʱ������us-��ʱʱ�� */
void delay_us(u16 us)
{
	while (us--)
	{
		delay_qus();
		delay_qus();
		delay_qus();
		delay_qus();
	}
}

/* ������ʱ������ms-��ʱʱ�� */
void delay_ms(u16 ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}
