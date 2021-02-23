/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：BSP_Config.c
 * 项目描述 ：恒沁华旭刷卡终端设备    
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 版  本   ：V2.0
 * 时  间   ：2021-01-25
**********************************************************************************/
#include "BSP_Config.h"
/*******************************************************************************
* 函 数 名         : BSP_Init
* 函数功能		     : 板件功能初始化配置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void BSP_Init(void)
{
	RCC_Configuration();               //开启外部时钟
	NVIC_Configuration();              //中断响应配置
	delay_init();                      //延时初始化 
  //设备初始化
  AT24C02_Init();                    //储存器AT24C02初始化
	PAM8406_Init();                    //功放PAM8406初始化
	HDMI_Init(9600);                   //串口3与液晶屏通讯初始化
  M6312_Init(115200,0,0);	           //串口1与M6312通讯初始化
	PC_Init(9600,0,1);                 //串口2或者USB跟电脑通讯初始化
	HXCard_Init(0,2);                  //华旭读卡中断初始化
	SCCard_Init(0,2);                  //三川读卡中断初始化
	//设备默认先关闭         
	//PC_Close();
	PAM8406_Close();
	//设备开始工作
	//PC_Open();                         //微电脑开机
	AT24C02IPTid_Init();               //初始化获取机器号、服务器IPPORT
  M6312_Connect(DeviceInfo.ServerInfo.ServerIP, DeviceInfo.ServerInfo.ServerPort, "网络连接中...");  //M6312连接服务器,信号强度检测，时间读取
	FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId); //第一次握手
	PC_Start();                        //电脑启动
	TIM_Heart_Init(9999,7199,1,0);     //心跳定时器时基1s，600秒握手一次
	//delay_ms(1000);
	HDMIShowMenuInfo(DeviceInfo);        //主界面显示
	HXCardIsInter = 0;                   //刷卡标志位，机器启动完成之前刷卡无效
	SCCardIsInter = 0;                   //刷卡标志位，机器启动完成之前刷卡无效
}






/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
