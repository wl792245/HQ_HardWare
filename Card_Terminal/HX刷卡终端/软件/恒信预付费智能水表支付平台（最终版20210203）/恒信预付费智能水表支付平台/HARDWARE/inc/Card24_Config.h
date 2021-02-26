/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Card24_Config.h
 * ��Ŀ���� ��24���ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��24����Card24_SCL--PB6  Card24_SDA--PB7	Card24_POWER--PB5 Card24_ICSW--PB9
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#ifndef __CARD24_CONFIG_H_
#define __CARD24_CONFIG_H_

#include "stm32f10x.h"
#include "IIC_Card.h"



//24����������
#define Card24_SDA_OUT     I2C_SDA_OUT()
#define Card24_SCL_OUT     I2C_SCL_OUT()
#define Card24_SDA_IN      I2C_SDA_IN()
#define Card24_SCL_IN      I2C_SCL_IN()
//24���������ø���
#define Card24_SCL_L       I2C_SCL_L
#define Card24_SCL_H       I2C_SCL_H
#define Card24_SDA_L       I2C_SDA_L
#define Card24_SDA_H       I2C_SDA_H
//24�����Ÿ���
#define Card24_I2C_SCL     I2C_SCL    //24��SCL����
#define Card24_I2C_SDA     I2C_SDA    //24��SDA����
#define Card24_GPIO        GPIO_I2C   

#define Card24_POWER       GPIO_Pin_5 //24����Դ��
//24����Դ��������
#define Card24_POWER_H     GPIO_SetBits(Card24_GPIO,Card24_POWER)
#define Card24_POWER_L     GPIO_ResetBits(Card24_GPIO,Card24_POWER)
#define Card24_SDARI       (GPIO_ReadInputDataBit(Card24_GPIO,Card24_I2C_SDA)!=0)
//24��Ԥ������Ϣ�ṹ��
typedef struct C24prepaymentInfo	
{
	u8 cardId[4];//����
	u8 cflag;//����־λ
	u8 areacode[3];//������
	u8 watersalesamount[2];//��ˮλ���֣�
	u8 watersalesamountlater[1];//��ˮλС������
	u8 securitybit[2];//����λ
	u8 times;//��ˮ����	
}Card24_PreInfo;

//24��������Ϣ�ṹ��
typedef struct C24ladderpaymentInfo
{
	u8 cardId[4];//����
	u8 cflag;//����־λ
	u8 areacode[3];//������
	u8 watersalesamount[4];//��ˮ��֣�
	u8 ladderprice[8];//��		
	u8 ladderamount[6];//��	
}Card24_LadderInfo;




void Card24_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);


void Card24_PreReadCID(Card24_PreInfo *Card24pre);
u8 Card24_PreWriteSaleAmount(u8 salesamount[2],u8 securitybit[2],u8 times );

void Card24_LadderReadCID(Card24_LadderInfo *Card24ladder);
u8 Card24_LadderWrite(u8 watersalesamount[4],u8 ladderprice[8],u8 ladderamount[6] );

#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
