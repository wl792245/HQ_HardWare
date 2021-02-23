/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：AT24C02.h
 * 项目描述 ：24C02外部接口调用       
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：24C02_SCL-PB2 24C02_SDA-PB0
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#ifndef __AT24C02_H_
#define __AT24C02_H_

#include "stm32f10x.h"

#define FISRT "Hx"
#define FTerminalIDServerIP "1912W200xx%89860411101850078642%V3.1%V1.9%121.196.181.103%03306%xh2xqWfEYjodrSPK90jU%"      


//选择卡类型
typedef enum CardType
{
	HX4442 = 0,  //华旭4442卡
	HX4428,      //华旭4428卡
	XT5557,      //新天5557卡
	SC5557,      //三川5557卡
//	YZ4424,      //扬州4424卡
//	YZ4442,      //扬州4442卡
//	YZ5557,      //扬州5557卡
}CardType_Info;

//设备日期
typedef struct Data
{
	char Year[3];			               //年
	char Mounth[3];			             //月
	char Day[3];				             //日
	char Hour[3];			               //时
	char Minute[3];			             //分
}DataInit, *PDataInit;
//设备信息结构体定义
typedef struct Terminal
{
	char TerminalId[15];			       //机器号
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //屏幕版本号
	char TerminalV[5];				       //设备版本号
	char QRcode[26];				         //二维码
	char SignalStrength[3];			     //信号强度
	char Fault;			                 //故障类型
}TerminalInit;

//服务器信息结构体定义
typedef struct Server
{
	char ServerIP[18];				       //服务器IP
	char ServerPort[6];			       //服务器端口号
}ServerInit;

typedef struct Device
{
	DataInit   DataInfo;
	ServerInit ServerInfo;
	TerminalInit TerminalInfo;
}DeviceInit, *pDeviceInit;

extern DeviceInit DeviceInfo;

void AT24C02_Init(void);
void AT24C02_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);
u8 AT24C02_Write(u16 WriteAddr,char *pBuffer,u16 NumToWrite);
void AT24C02IPTid_Init(void);
#endif 
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
