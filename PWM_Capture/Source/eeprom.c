#include "i2c.h"
#include "eeprom.h"

/* ��ȡEEPROM�е�һ���ֽڣ�addr-�ֽڵ�ַ */
u8 E2ReadByte(u8 addr)
{
    u8 dat;
    
    do { //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д
        I2CStart();
        if (I2CWrite(0x50<<1)) //Ѱַ������Ӧ��������ѭ�������������ѯ
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);           //д��洢��ַ
    I2CStart();               //�����ظ������ź�
    I2CWrite((0x50<<1)|0x01); //Ѱַ����������Ϊ������
    dat = I2CReadNAK();       //��ȡһ���ֽ�����
    I2CStop();
    
    return dat;
}

/* ��EEPROM��д��һ���ֽڣ�addr-�ֽڵ�ַ */
void E2WriteByte(u8 addr, u8 dat)
{
    do { //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д
        I2CStart();
        if (I2CWrite(0x50<<1)) //Ѱַ������Ӧ��������ѭ�������������ѯ
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);  //д��洢��ַ
    I2CWrite(dat);   //д��һ���ֽ�����
    I2CStop();
}
