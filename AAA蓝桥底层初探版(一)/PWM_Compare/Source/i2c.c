#include "i2c.h"

void I2CInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��       
    GPIO_SetBits(GPIOB, GPIO_Pin_6|GPIO_Pin_7);           //SCL��SDA��ʼ����ߵ�ƽ(���������ŵ�ƽ���Ա���IO��ʼ�������п��ܲ�����ë��)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;  //ѡ��SCL��SDA����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;      //ѡ��©���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;     //�������10MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* ����������ʼ�ź� */
void I2CStart(void)
{
    I2C_SDA_OUT = 1; //����ȷ��SDA��SCL���Ǹߵ�ƽ
    I2C_SCL_OUT = 1;
    delay_us(5);
    I2C_SDA_OUT = 0; //������SDA
    delay_us(5);
    I2C_SCL_OUT = 0; //������SCL
}

/* ��������ֹͣ�ź� */
void I2CStop(void)
{
    I2C_SCL_OUT = 0; //����ȷ��SDA��SCL���ǵ͵�ƽ
    I2C_SDA_OUT = 0;
    delay_us(5);
    I2C_SCL_OUT = 1; //������SCL
    delay_us(5);
    I2C_SDA_OUT = 1; //������SDA
    delay_us(5);
}

/* I2C����д������dat-��д���ֽڣ�����ֵ-�ӻ�Ӧ��λ��ֵ */
u8 I2CWrite(u8 dat)
{
    int i;
    u8 ack;   //�����ݴ�Ӧ��λ��ֵ
    
    for (i=0; i<8; i++)  //ѭ����8bit���������������
    {
        I2C_SDA_OUT = (dat&0x80) ? 1 : 0; //�����λ��ֵ�����SDA��
        delay_us(5);
        I2C_SCL_OUT = 1; //����SCL
        delay_us(5);
        I2C_SCL_OUT = 0; //������SCL�����һ��λ����
        dat <<= 1;       //���ƽ��θ�λ��Ϊ���λ��ʵ�ָ�λ���ȵ�λ�ں�ķ���˳��
    }
    I2C_SDA_OUT = 1;  //8λ���ݷ�����������ͷ�SDA���Լ��ӻ�Ӧ��
    delay_us(5);
    I2C_SCL_OUT = 1;  //����SCL
    ack = I2C_SDA_IN; //��ȡ��ʱ��SDAֵ����Ϊ�ӻ���Ӧ��ֵ
    delay_us(5);
    I2C_SCL_OUT = 0;  //������SCL���Ӧ��λ��������ס����        
    delay_us(5);
    
    return (!ack); //Ӧ��ֵȡ���Է���ͨ�����߼���
                   //0=�����ڻ�æ��д��ʧ�ܣ�1=�����ҿ��л�д��ɹ�
}

/* I2C���߶�ȡ8λ���ݣ�����ֵ-�������ֽ� */
u8 I2CRead(void)
{
    int i;
    u8 dat = 0; //���ݽ��ձ�������ֵ0
    
    I2C_SDA_OUT = 1;    //����ȷ�������ͷ�SDA
    for (i=0; i<8; i++) //ѭ���������ϵ�8bit���ݶ���dat��
    {
        delay_us(5);
        I2C_SCL_OUT = 1;    //����SCL
        dat <<= 1;          //���ƽ���������λ���λ�ƶ���ʵ�ָ�λ���ȵ�λ�ں�Ľ���˳��
        if(I2C_SDA_IN != 0) //��ȡSDA��ֵ��dat���λ��
        {
            dat |= 0x01;    //SDAΪ1ʱ����dat���λΪ1��SDAΪ0ʱ�޲���������Ϊ��ʼֵ��0
        }
        delay_us(5);
        I2C_SCL_OUT = 0;    //������SCL����ʹ�ӻ����ͳ���һλ
    }
    return dat;
}

/* I2C���߶������������ͷ�Ӧ���źţ�����ֵ-�������ֽ� */
u8 I2CReadNAK(void)
{
    u8 dat;
    
    dat = I2CRead();  //��ȡ8λ����
    I2C_SDA_OUT = 1;  //8λ���ݶ�ȡ�������SDA�����ͷ�Ӧ���ź�
    delay_us(5);
    I2C_SCL_OUT = 1;  //����SCL
    delay_us(5);
    I2C_SCL_OUT = 0;  //������SCL��ɷ�Ӧ��λ��������ס����
    delay_us(5);
    
    return dat;
}

/* I2C���߶�������������Ӧ���źţ�����ֵ-�������ֽ� */
u8 I2CReadACK(void)
{
    u8 dat;
    
    dat = I2CRead();  //��ȡ8λ����
    I2C_SDA_OUT = 0;  //8λ���ݶ�ȡ�������SDA������Ӧ���ź�
    delay_us(5);
    I2C_SCL_OUT = 1;  //����SCL
    delay_us(5);
    I2C_SCL_OUT = 0;  //������SCL���Ӧ��λ��������ס����
    delay_us(5);
    
    return dat;
}
