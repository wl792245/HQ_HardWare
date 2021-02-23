/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：HDMI_T050.h
 * 项目描述 ：液晶屏T050与串口5通讯     
 * 开发平台 ：基于STM32F103VBT6芯片开发
 * 硬件连接 ：HDMI_RX-PB10(TX)   HDMI_TX-PB11(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-8
**********************************************************************************/
#ifndef __HDMI_T050_H_
#define __HDMI_T050_H_

#include "stm32f10x.h"
#include "AT24C02.h"
#include "Usart3_HDMI.h"

#define  USART_HDMI_End  USART_HDMI_SendB(0xff,3)  //结束符

/*******************************************************************************
 userId：用户ID
 userAdress：用户地址
 waterProperties：用水性质
 totalAmount：累计购买总量
 LastAllBuyMount：当年购买总量
 Unit:单位 阶梯 Unit=1  显示 元；  预付费   Unit=0  显示  吨	
 islongthan12：字符长度超过12滚动显示
*******************************************************************************/
//用户信息
typedef struct UserType
{
	char *userId;
	char *userAdress;
}User;
//购水信息
typedef struct WaterType
{
	char *waterProperties;
	char *totalAmount;
	char *LastAllBuyMount;
	int Unit;
}Water;
//page4上显示
typedef struct Show
{
	User UserInfo;
	Water WaterInfo;
	int islongthan12;
}UserInfoShow, *PUserInfoShow;

void HDMI_Init(u32 Bound);
void HDMI_CMD_Send(char *cmd);

void NetSignalDisplay(u8 SignalValue);
void HDMISendConncetOK(void);
void HDMIShowRechargeSucess(char *RechargeSucessValue,int Unit);
void HDMIShowUserDetail(UserInfoShow UserInfoDisplay);
void HDMIShowInfo(char *reason);
void HDMIShowWait(void);
void HDMIShowWaitEnd(void);
void HDMIShowChangeSuccess(void);
void HDMIShowSignal(unsigned char SignalValue);
void HDMIShowServerInfo(pDeviceInit deviceinfo);
void HardWareErr(void);
void HDMIShowMenuInfo(DeviceInit Menu);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
