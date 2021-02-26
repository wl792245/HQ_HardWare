/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：IIC_Card.h
 * 项目描述 ：IIC通讯外部接口声明     
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：PB6(SCL) PB7(SDA)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
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
/* 声明全局函数 */
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







/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
