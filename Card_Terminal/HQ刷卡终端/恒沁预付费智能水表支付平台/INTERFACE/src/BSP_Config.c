/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：BSP_Config.c
 * 项目描述 ：恒沁华旭刷卡终端设备    
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 版  本   ：V2.0
 * 时  间   ：2021-01-25
**********************************************************************************/
#include "BSP_Config.h"
#include "string.h"

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
	PC_Close();
	PAM8406_Close();
	//设备开始工作
	PC_Open();                         //微电脑开机
	AT24C02IPTid_Init();               //初始化获取机器号、服务器IPPORT
  M6312_Connect(&DeviceInfo, "网络连接中...");  //M6312连接服务器,信号强度检测，时间读取
	FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId); //第一次握手
	PC_Start();                        //电脑启动
	TIM_Heart_Init(9999,7199,1,0);     //心跳定时器时基1s，600秒握手一次
	HDMIShowMenuInfo(&DeviceInfo);     //主界面显示
	HXCardIsInter = 0;                 //刷卡标志位，机器启动完成之前刷卡无效
	SCCardIsInter = 0;                 //刷卡标志位，机器启动完成之前刷卡无效
}
/*******************************************************************************
* 函 数 名         : Remote_ChangeIp
* 函数功能		     : 远程更改IP、端口、二维码
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Remote_ChangeIp(void)
{
	unsigned int Remote_Num[10];
	char  QRcode[21];
	Card_Server  Remote_CardChange;
	M6312_RestartFlag = 0;
	if (usart1_rcv_len > 0)
	{
		String_Clear(TerminalIDServerIP, 100);
		M6312_USART_GetRcvData(TerminalIDServerIP, usart1_rcv_len);
		Get_Head(Remote_CardChange.Card_ServerRecHead, TerminalIDServerIP, '%', Remote_Num, 10); //取头
		if (strcmp(Remote_CardChange.Card_ServerRecHead,"ChangePc")==0) //更改IP
		{
			PC_SendData("HelloMPC%$$");  //电脑重新计时
			String_Find(QRcode, TerminalIDServerIP, Remote_Num[3]+1, Remote_Num[4]);          //获取二维码
			if(strlen(QRcode)>10)
			{
				String_Find(DeviceInfo.TerminalInfo.QRcode, QRcode, 0, strlen(QRcode));
				AT24C02_Write(69,DeviceInfo.TerminalInfo.QRcode,21);
				String_Find(DeviceInfo.ServerInfo.ServerIP, TerminalIDServerIP, Remote_Num[0]+1, Remote_Num[1]);         //获取服务器IP号
				AT24C02_Write(47,DeviceInfo.ServerInfo.ServerIP,15);
				String_Find(DeviceInfo.ServerInfo.ServerPort, TerminalIDServerIP, Remote_Num[1]+1, Remote_Num[2]);        //获取服务器端口号
				AT24C02_Write(63,DeviceInfo.ServerInfo.ServerPort,5);
				String_Find(DeviceInfo.TerminalInfo.TerminalId, TerminalIDServerIP, Remote_Num[2]+1, Remote_Num[3]);        //获取机器号
				AT24C02_Write(5,DeviceInfo.TerminalInfo.TerminalId,10);
				HDMIShowInfo("IP二维码更改成功");
				PC_StartPam("IP二维码更改成功");
				M6312_RestartFlag = 0x63;
			}
			else
			{
				HDMIShowInfo("二维码更改失败,请重试");
				PC_StartPam("二维码更改失败,请重试");
			}
		}
	}
}


/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
