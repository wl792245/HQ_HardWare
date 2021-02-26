/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��IIC_Card.h
 * ��Ŀ���� ��IICͨѶ�ⲿ�ӿ�����     
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��PB6(SCL) PB7(SDA)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/

#ifndef __IIC_CARD_H_
#define __IIC_CARD_H_

#include "stm32f10x.h"

#define I2C_SCL     GPIO_Pin_6	  //PB6
#define I2C_SDA     GPIO_Pin_7	  //PB7
#define I2C_POWER   GPIO_Pin_5
#define I2C_ICSW    GPIO_Pin_9 
#define GPIO_I2C    GPIOB

#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)
#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)

#define POWER_H   GPIO_SetBits(GPIO_I2C,I2C_POWER)
#define POWER_L   GPIO_ResetBits(GPIO_I2C,I2C_POWER)
/* ����ȫ�ֺ��� */
void I2C_INIT(void);
void I2C_SCL_OUT(void);
void I2C_SDA_OUT(void);
void I2C_SDA_IN(void);
void I2C_SCL_IN(void);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_Ack(void);
void I2C_NAck(void);
u8   I2C_Wait_Ack(void);
void I2C_Send_Byte(u8 txd);
u8   I2C_Read_Byte(u8 ack);

#endif







/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
