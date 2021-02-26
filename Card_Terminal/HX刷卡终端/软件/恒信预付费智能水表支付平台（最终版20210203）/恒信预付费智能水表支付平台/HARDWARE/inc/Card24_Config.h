/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card24_Config.h
 * 项目描述 ：24卡接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：24卡：Card24_SCL--PB6  Card24_SDA--PB7	Card24_POWER--PB5 Card24_ICSW--PB9
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#ifndef __CARD24_CONFIG_H_
#define __CARD24_CONFIG_H_

#include "stm32f10x.h"
#include "IIC_Card.h"



//24卡函数复用
#define Card24_SDA_OUT     I2C_SDA_OUT()
#define Card24_SCL_OUT     I2C_SCL_OUT()
#define Card24_SDA_IN      I2C_SDA_IN()
#define Card24_SCL_IN      I2C_SCL_IN()
//24卡引脚设置复用
#define Card24_SCL_L       I2C_SCL_L
#define Card24_SCL_H       I2C_SCL_H
#define Card24_SDA_L       I2C_SDA_L
#define Card24_SDA_H       I2C_SDA_H
//24卡引脚复用
#define Card24_I2C_SCL     I2C_SCL    //24卡SCL引脚
#define Card24_I2C_SDA     I2C_SDA    //24卡SDA引脚
#define Card24_GPIO        GPIO_I2C   

#define Card24_POWER       GPIO_Pin_5 //24卡电源脚
//24卡电源引脚设置
#define Card24_POWER_H     GPIO_SetBits(Card24_GPIO,Card24_POWER)
#define Card24_POWER_L     GPIO_ResetBits(Card24_GPIO,Card24_POWER)
#define Card24_SDARI       (GPIO_ReadInputDataBit(Card24_GPIO,Card24_I2C_SDA)!=0)
//24卡预付费信息结构体
typedef struct C24prepaymentInfo	
{
	u8 cardId[4];//卡号
	u8 cflag;//卡标志位
	u8 areacode[3];//区域码
	u8 watersalesamount[2];//售水位（吨）
	u8 watersalesamountlater[1];//售水位小数部分
	u8 securitybit[2];//保密位
	u8 times;//售水次数	
}Card24_PreInfo;

//24卡阶梯信息结构体
typedef struct C24ladderpaymentInfo
{
	u8 cardId[4];//卡号
	u8 cflag;//卡标志位
	u8 areacode[3];//区域码
	u8 watersalesamount[4];//售水额（分）
	u8 ladderprice[8];//价		
	u8 ladderamount[6];//量	
}Card24_LadderInfo;




void Card24_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);


void Card24_PreReadCID(Card24_PreInfo *Card24pre);
u8 Card24_PreWriteSaleAmount(u8 salesamount[2],u8 securitybit[2],u8 times );

void Card24_LadderReadCID(Card24_LadderInfo *Card24ladder);
u8 Card24_LadderWrite(u8 watersalesamount[4],u8 ladderprice[8],u8 ladderamount[6] );

#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
