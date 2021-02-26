/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Card42_Config.h
 * ��Ŀ���� ��4442������IICͨѶ�ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��42����Card42_SCL--PB6  Card42_SDA--PB7	Card42_POWER--PB5 Card42_ICSW--PB9
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __CARD42_CONFIG_H_
#define __CARD42_CONFIG_H_
#include "stm32f10x.h"
#include "IIC_Card.h"



//24����������
#define Card42_SDA_OUT     I2C_SDA_OUT()
#define Card42_SCL_OUT     I2C_SCL_OUT()
#define Card42_SDA_IN      I2C_SDA_IN()
#define Card42_SCL_IN      I2C_SCL_IN()
//24���������ø���
#define Card42_SCL_L       I2C_SCL_L
#define Card42_SCL_H       I2C_SCL_H
#define Card42_SDA_L       I2C_SDA_L
#define Card42_SDA_H       I2C_SDA_H
//24�����Ÿ���
#define Card42_I2C_SCL     I2C_SCL    //24��SCL����
#define Card42_I2C_SDA     I2C_SDA    //24��SDA����
#define Card42_GPIO        GPIO_I2C   

#define Card42_POWER       GPIO_Pin_5 //24����Դ��
//24����Դ��������
#define Card42_POWER_H     GPIO_SetBits(Card42_GPIO,Card42_POWER)
#define Card42_POWER_L     GPIO_ResetBits(Card42_GPIO,Card42_POWER)
#define Card42_SDARI       (GPIO_ReadInputDataBit(Card42_GPIO,Card42_I2C_SDA)!=0)
//4442��Ԥ֧����Ϣ
typedef struct C4442prepaymentInfo
{
	u8 cardId[4];               //���� 
	u8 cflag;                   //����־λ
	u8 areacode[2];             //������
	u8 watersalesamount[3];     //����ˮ��������
}Card42_PreInfo;



void Card42_PreReadCID(Card42_PreInfo *Card4442);
unsigned char Card42_Writesalesamount(unsigned char  *salesamount);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
