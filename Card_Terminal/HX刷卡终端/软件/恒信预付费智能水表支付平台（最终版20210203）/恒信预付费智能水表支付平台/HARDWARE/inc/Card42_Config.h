/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card42_Config.h
 * 项目描述 ：4442卡调用IIC通讯接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：42卡：Card42_SCL--PB6  Card42_SDA--PB7	Card42_POWER--PB5 Card42_ICSW--PB9
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#ifndef __CARD42_CONFIG_H_
#define __CARD42_CONFIG_H_
#include "stm32f10x.h"
#include "IIC_Card.h"



//24卡函数复用
#define Card42_SDA_OUT     I2C_SDA_OUT()
#define Card42_SCL_OUT     I2C_SCL_OUT()
#define Card42_SDA_IN      I2C_SDA_IN()
#define Card42_SCL_IN      I2C_SCL_IN()
//24卡引脚设置复用
#define Card42_SCL_L       I2C_SCL_L
#define Card42_SCL_H       I2C_SCL_H
#define Card42_SDA_L       I2C_SDA_L
#define Card42_SDA_H       I2C_SDA_H
//24卡引脚复用
#define Card42_I2C_SCL     I2C_SCL    //24卡SCL引脚
#define Card42_I2C_SDA     I2C_SDA    //24卡SDA引脚
#define Card42_GPIO        GPIO_I2C   

#define Card42_POWER       GPIO_Pin_5 //24卡电源脚
//24卡电源引脚设置
#define Card42_POWER_H     GPIO_SetBits(Card42_GPIO,Card42_POWER)
#define Card42_POWER_L     GPIO_ResetBits(Card42_GPIO,Card42_POWER)
#define Card42_SDARI       (GPIO_ReadInputDataBit(Card42_GPIO,Card42_I2C_SDA)!=0)
//4442卡预支付信息
typedef struct C4442prepaymentInfo
{
	u8 cardId[4];               //卡号 
	u8 cflag;                   //卡标志位
	u8 areacode[2];             //区域码
	u8 watersalesamount[3];     //售入水量（升）
}Card42_PreInfo;



void Card42_PreReadCID(Card42_PreInfo *Card4442);
unsigned char Card42_Writesalesamount(unsigned char  *salesamount);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
