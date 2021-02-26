/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：IIC_24C02.h
 * 项目描述 ：IIC外部接口调用       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：PB10(SCL) PB11(SDA)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#ifndef __IIC_24C02_H_
#define __IIC_24C02_H_

#include "stm32f10x.h"

#define IIC_SCL GPIO_Pin_10	  //PB10
#define IIC_SDA GPIO_Pin_11	  //PB11
#define GPIO_IIC GPIOB

#define IIC_SCL_H GPIO_SetBits(GPIO_IIC,IIC_SCL)
#define IIC_SCL_L GPIO_ResetBits(GPIO_IIC,IIC_SCL)

#define IIC_SDA_H GPIO_SetBits(GPIO_IIC,IIC_SDA)
#define IIC_SDA_L GPIO_ResetBits(GPIO_IIC,IIC_SDA)

/* 函数声明 */
void IIC_INIT(void);
void IIC_SDA_OUT(void);
void IIC_SDA_IN(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(void);
void IIC_NAck(void);
u8   IIC_Wait_Ack(void);
void IIC_Send_Byte(u8 txd);
u8   IIC_Read_Byte(u8 ack);

#endif 
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
