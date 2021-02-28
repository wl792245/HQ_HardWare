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

#define FISRT "HX"
#define FTerminalIDServerIP "1912W200xx%89860411101850078642%V3.1%V1.9%121.196.181.103%03306%xh2xqWfEYjodrSPK90jU%"

//设备信息结构体定义
typedef struct Terminal
{
	char TerminalId[15];			       //机器号
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //屏幕版本号
	char TerminalV[5];				       //设备版本号
	char QRcode[26];				         //二维码
	char SignalStrength[3];			     //信号强度
	char ReStart[5];                 //网络重连次数
}TerminalInit;

//服务器信息结构体定义
typedef struct Server
{
	char ServerIP[18];				       //服务器IP
	char ServerPort[6];			       //服务器端口号
}ServerInit;

typedef struct Device
{
	ServerInit ServerInfo;
	TerminalInit TerminalInfo;
}DeviceInit, *pDeviceInit;

extern DeviceInit DeviceInfo;
extern volatile int Restart_Count;

void AT24C02_Init(void);
void AT24C02_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);
u8 AT24C02_Write(u16 WriteAddr,char *pBuffer,u16 NumToWrite);
void AT24C02IPTid_Init(void);
void ResStart_Write(char * ReStartInfo, int value);
int ReStart_Read(char *ReStartInfo);
#endif 
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
