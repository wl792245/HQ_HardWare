/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：HDMI_T050.c
 * 项目描述 ：液晶屏T050与串口5通讯设计  
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-8
**********************************************************************************/
#include "HDMI_T050.h"
#include "string.h"
#include "String_Get.h"
#include "M6311.h"
//页面选择
#define PAGE0   "page 0"
#define PAGE1   "page 1"
#define PAGE2   "page 2"
#define PAGE3   "page 3"
#define PAGE4   "page 4"
//
/*******************************************************************************
* 函 数 名         : HDMI_Init
* 函数功能		     : 液晶屏初始化
* 输    入         : Bound：波特率， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	UART_HDMI_Init(Bound, m_irq, s_irq);
}
/*******************************************************************************
* 函 数 名         : HDMI_CMD_Send
* 函数功能		     : 指令发送
* 输    入         : cmd：指令
* 输    出         : 无
*******************************************************************************/
void HDMI_CMD_Send(char *cmd)
{
	UART_HDMI_SendS(cmd);
	UART_HDMI_End;
}
/*******************************************************************************
* 函 数 名         : HDMI_ShowInfo
* 函数功能		     : 屏幕信息显示
* 输    入         : pDest:数组选择；pControl：控件选择 pString：输入字符
* 输    出         : 无
*******************************************************************************/
static void HDMI_ShowInfo(char *pDest, char *pControl, char *pString)
{
	strcpy(pDest,pControl);
	strcat(pDest,pString);
	strcat(pDest," \"");
  HDMI_CMD_Send(pDest);
}
/*******************************************************************************
* 函 数 名         : HDMI_UnitShowInfo
* 函数功能		     : 屏幕带单位信息显示
* 输    入         : pDest:数组选择；pControl：控件选择 pString：输入字符, unit:单位
* 输    出         : 无
*******************************************************************************/
static void HDMI_UnitShowInfo(char *pDest, char *pControl, char *pString, int unit)
{
	strcpy(pDest,pControl);
	strcat(pDest,pString);
	if(unit==0)
	{
		strcat(pDest," 元\"");	
	}
	else
	{
	  strcat(pDest," 吨\"");
	}
	HDMI_CMD_Send(pDest);
}
/*******************************************************************************
* 函 数 名         : NetSignalDisplay
* 函数功能		     : 网络信号强度显示
* 输    入         : SignalValue:输入信号强度值，显示对应的图片
* 输    出         : 无
*******************************************************************************/
void NetSignalDisplay(u8 SignalValue) 
{
	if((SignalValue>0)&&(SignalValue<=5))//根据信号强度在屏幕上显示信号条
	{	
	  HDMI_CMD_Send("p1.pic=10");
	}
	else if((SignalValue>5)&&(SignalValue<=11))//根据信号强度在屏幕上显示信号条
	{					
		HDMI_CMD_Send("p1.pic=11");			
	}
	else if((SignalValue>11)&&(SignalValue<=17))//根据信号强度在屏幕上显示信号条
	{					
		HDMI_CMD_Send("p1.pic=12");			
	}
	else if((SignalValue>17)&&(SignalValue<=23))//根据信号强度在屏幕上显示信号条
	{					
		HDMI_CMD_Send("p1.pic=13");				
	}
	else if((SignalValue>23)&&(SignalValue<=31))//根据信号强度在屏幕上显示信号条
	{					
		HDMI_CMD_Send("p1.pic=14");		
	}
	else
	{
    HDMI_CMD_Send("p1.pic=10");	
	}
}
/*******************************************************************************
* 函 数 名         : HDMISendNetInit
* 函数功能		     : 液晶屏显示网络初始化中
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HDMISendNetInit(void)  
{
	NetSignalDisplay(0);
	HDMI_CMD_Send(PAGE0);
	HDMI_CMD_Send("t0.txt=\"网络初始化中。。。。\"");
} 	
/*******************************************************************************
* 函 数 名         : HDMISendConncetOK
* 函数功能		     : 液晶屏连接成功显示
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HDMISendConncetOK(void)         
{ 

	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_CMD_Send("t0.txt=\"连接成功！！\"");
	//信号强度转换函数
	//NetSignalDisplay(0);
}
/*******************************************************************************
* 函 数 名         : HDMIShowRechargeSucess
* 函数功能		     : 液晶屏显示充值成功
* 输    入         : RechargeSucessValue:数量。uint :单位 
* 输    出         : 无
*******************************************************************************/
void HDMIShowRechargeSucess(char *RechargeSucessValue,int Unit)    
{
	char SucessString[50];
	String_Clear(SucessString, 50);
	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_UnitShowInfo(SucessString, "t0.txt=\"您已成功充值 ",RechargeSucessValue,Unit);
}
/*******************************************************************************
* 函 数 名         : HDMIShowUserDetail
* 函数功能		     : 液晶屏显示用户信息
* 输    入         : userId：用户ID
										 userAdress：用户地址
                     userProperties：用水性质
										 totalAmount：累计购买总量
                     LastAllBuyMount：当年购买总量
										 Unit:单位 阶梯 Unit=0  显示 元；  预付费   Unit=1  显示  吨	
                     islongthan12：
* 输    出         : 无
*******************************************************************************/
void HDMIShowUserDetail(UserInfoShow UserInfoDisplay)
{
	char SucessString[50]={'\0'};
	HDMI_CMD_Send(PAGE4);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(SucessString, "t4.txt=\"",UserInfoDisplay.UserInfo.userId);
	if(UserInfoDisplay.islongthan12 == 1)
	{
		HDMI_CMD_Send("vis g1,1");
		HDMI_CMD_Send("vis t5,0");
		String_Clear(SucessString,50);
		HDMI_ShowInfo(SucessString, "g1.txt=\"",UserInfoDisplay.UserInfo.userAdress);
	}
	else
	{
		HDMI_CMD_Send("vis g1,0");
		HDMI_CMD_Send("vis t5,1");
		String_Clear(SucessString,50);
		HDMI_ShowInfo(SucessString, "t5.txt=\"",UserInfoDisplay.UserInfo.userAdress);
	}
	String_Clear(SucessString,50);
	HDMI_ShowInfo(SucessString, "t0.txt=\"",UserInfoDisplay.WaterInfo.waterProperties);
	String_Clear(SucessString,50);
	HDMI_UnitShowInfo(SucessString, "t6.txt=\"",UserInfoDisplay.WaterInfo.totalAmount,UserInfoDisplay.WaterInfo.Unit);
  String_Clear(SucessString,50);
	HDMI_UnitShowInfo(SucessString, "t9.txt=\"",UserInfoDisplay.WaterInfo.LastAllBuyMount,1);
	//信号强度
}
/*******************************************************************************
* 函 数 名         : HDMIShowFail
* 函数功能		     : 液晶屏显示失败原因
* 输    入         : reason:原因
* 输    出         : 无
*******************************************************************************/
void HDMIShowInfo(char *reason) 
{
	char reasonstr[50]={'\0'};
	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(reasonstr, "t0.txt=\"",reason);
}	
/*******************************************************************************
* 函 数 名         : HDMIShowWait
* 函数功能		     : 液晶屏显示等待
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HDMIShowWait(void)
{
	HDMI_CMD_Send(PAGE2);
	HDMIShowSignal(M6311_Signal);
}	

/*******************************************************************************
* 函 数 名         : HDMIShowChangeSuccess
* 函数功能		     : 液晶屏显示更改成功
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HDMIShowChangeSuccess(void)         
{ 

	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_CMD_Send("t0.txt=\"更改成功\"");
}
/*******************************************************************************
* 函 数 名         : HDMIShowServerInfo
* 函数功能		     : 液晶屏显示服务器信息
* 输    入         : deviceinfo：设备信息以及服务器信息
* 输    出         : 无
*******************************************************************************/
void HDMIShowServerInfo(pDeviceInit deviceinfo)  
{
	char sysMsg[50];
	String_Clear(sysMsg, 50);
	HDMI_CMD_Send(PAGE3);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(sysMsg, "t1.txt=\"",deviceinfo->TerminalInfo.TerminalId);
	HDMI_ShowInfo(sysMsg, "t3.txt=\"",deviceinfo->TerminalInfo.ICCID);
	HDMI_ShowInfo(sysMsg, "t5.txt=\"",deviceinfo->ServerInfo.ServerIP);
	HDMI_ShowInfo(sysMsg, "t7.txt=\"",deviceinfo->ServerInfo.ServerPort);
	HDMI_ShowInfo(sysMsg, "t9.txt=\"",deviceinfo->TerminalInfo.ScreenV);
	HDMI_ShowInfo(sysMsg, "t11.txt=\"",deviceinfo->TerminalInfo.TerminalV);
	HDMI_ShowInfo(sysMsg, "t13.txt=\"",deviceinfo->TerminalInfo.SignalStrength);
}
/*******************************************************************************
* 函 数 名         : HDMIShowMenuInfo
* 函数功能		     : 液晶屏显示主界面信息
* 输    入         : deviceinfo：取二维码信息
* 输    出         : 无
*******************************************************************************/
void HDMIShowMenuInfo(char * QRcode)
{
 	char sysMsg[100];
  String_Clear(sysMsg,100);
	HDMI_CMD_Send(PAGE0);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(sysMsg, "qr0.txt=\"http://weixin.qq.com/r/",QRcode);//44个元素，对应QRcode[44]//https://u.wechat.com/EEAAXRbIIE0YxMhmj9CmUCU//\"http://weixin.qq.com/r/
}
/*******************************************************************************
* 函 数 名         : HDMIShowSignal
* 函数功能		     : 液晶屏显示信号强度
* 输    入         : SignalValue:信号值
* 输    出         : 无
*******************************************************************************/
void HDMIShowSignal(unsigned char SignalValue)
{
	if((SignalValue>0)&&(SignalValue<=5))//根据信号强度在屏幕上显示信号条
	{
		HDMI_CMD_Send("p1.pic=10");					
	}
	else if((SignalValue>5)&&(SignalValue<=11))//根据信号强度在屏幕上显示信号条
	{
		HDMI_CMD_Send("p1.pic=11");					
	}
	else if((SignalValue>11)&&(SignalValue<=17))//根据信号强度在屏幕上显示信号条
	{
		HDMI_CMD_Send("p1.pic=12");					
	}
	else if((SignalValue>17)&&(SignalValue<=23))//根据信号强度在屏幕上显示信号条
	{
		HDMI_CMD_Send("p1.pic=13");					
	}
	else if((SignalValue>23)&&(SignalValue<=31))//根据信号强度在屏幕上显示信号条
	{
		HDMI_CMD_Send("p1.pic=14");					
	}
	else
	{
		;	
	}
}
/*******************************************************************************
* 函 数 名         : HardWareErr
* 函数功能		     : 液晶屏显示硬件错误，联系厂家
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HardWareErr(void)
{
	HDMI_CMD_Send(PAGE1);
	HDMI_CMD_Send("t0.txt=\"硬件问题,联系厂家\"");
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
