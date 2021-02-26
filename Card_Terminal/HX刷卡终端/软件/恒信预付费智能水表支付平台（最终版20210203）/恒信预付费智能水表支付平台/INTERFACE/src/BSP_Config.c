/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：BSP_Config.c
 * 项目描述 ：板上硬件接口设计       
 * 开发平台 ：基于STM32F103C8T6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2020-10-29
**********************************************************************************/
#include "BSP_Config.h"


#define FirstDownLoad 0  //第一次下载，需要配置IP和PORT
#define ServerIPPORT  "121.196.181.103%03306%"
#define CID           "1906W10012%00000000000000000000%"
/*******************************************************************************
* 函 数 名         : BSP_Init
* 函数功能		     : 板件功能初始化配置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void BSP_Init(void)
{
	M6311_Signal = 31;
	RCC_DeInit();			                 //复位时钟
	SystemInit();	                     //系统时钟初始化
	RCC_Configuration();               //开启外部时钟
	NVIC_Configuration();              //中断响应配置
	delay_init();                      //延时初始化                
	AT24C02_Init();                    //AT24C02初始化                     
	Card57_Init(1,0);	                 //57卡初始化
	Card_Init(1,0);                    //24.42卡初始化
  XFS_Init(115200,3,0);              //语音-串口4初始化 
	HDMI_Init(9600,3,1);               //液晶屏-串口5初始化         
  M6311_Init(115200,2,0);            //2G串-口2初始化
	HDMIShowInfo("系统初始化完成");
	#if FirstDownLoad
	while (AT24C02_Write(47,ServerIPPORT,22))
	{
		XFS_Only_Play("服务器IP更改失败");
	}
	AT24C02_Write(5,CID,32);
	#endif
	delay_ms(1000); 
	AT24C02IPTid_Init();	//初始化获取机器号服务器IP
	M6311_Connect(DeviceInfo.ServerInfo.ServerIP, DeviceInfo.ServerInfo.ServerPort, "网络连接中...");  //M6311连接服务器
	M6311_Signal =M6311_SignalQuery(DeviceInfo.TerminalInfo.SignalStrength);
	HDMISendConncetOK();
	M6311_Connecting = 0;
	FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId);
	CacheOrder_update();               //账单缓存更新
	TIM_Heart_Init(9999,7199,1,0);     //心跳定时器时基1s，600秒握手一次
	delay_ms(1000);
	HDMIShowMenuInfo(DeviceInfo.TerminalInfo.QRcode);
	//IWDG_Init();                     //独立看门狗，8S之内喂一次就行
	CardIsInter = 0;                 //置不在刷卡标志，允许刷卡
	Card57_Flag = 0;
}

/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************/
