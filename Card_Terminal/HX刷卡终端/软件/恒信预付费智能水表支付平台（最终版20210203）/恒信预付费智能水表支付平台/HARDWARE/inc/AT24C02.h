/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：AT24C02.h
 * 项目描述 ：24C02外部接口调用       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：24C02_SCL-PB10 24C02_SDA-PB11
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#ifndef __AT24C02_H_
#define __AT24C02_H_

#include "stm32f10x.h"


//设备信息结构体定义
typedef struct Terminal
{
	char TerminalId[20];			       //机器号
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //屏幕版本号
	char TerminalV[5];				       //设备版本号
	char QRcode[44];				         //二维码
	char SignalStrength[3];			     //信号强度
	char ReStartTime;                //重启次数
}TerminalInit;

//服务器信息结构体定义
typedef struct Server
{
	char ServerIP[30];				       //服务器IP
	char ServerPort[10];			       //服务器端口号
}ServerInit;

typedef struct Device
{
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
