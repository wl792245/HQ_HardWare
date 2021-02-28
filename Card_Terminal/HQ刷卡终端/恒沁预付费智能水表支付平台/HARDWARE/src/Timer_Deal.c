/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Timer_Deal.c
 * 项目描述 ：定时器处理函数设计
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Timer_Deal.h"
#include "String_Config.h"
//#include "AT24C02.h"
#include "string.h"
#include "M6312.h"
#include "systick.h"
#include "HDMI_T050.h"

///定时中断3，时间1秒。
///用作600秒SOCKET心跳，M6311访问
volatile u32  Server_Time=0;  //与服务器握手时间
volatile u32  PC_Time=0;      //与电脑握手时间
volatile u32  err=0;
volatile uint32_t   Server_TimeIsInter=0; //服务器定时器中断标志位
volatile uint32_t   PC_TimeIsInter=0; //服务器定时器中断标志位
static char UploadTimerBuf[50];
static char TimeSendBuf[50];
/*******************************************************************************
* 函 数 名         : FirstHeartOpen
* 函数功能		     : 向消息服务器发送第一次心跳	   
* 输    入         : 机器号TerminalId
* 输    出         : 无
*******************************************************************************/
void FirstHeart_Open(char *TerminalId)
{	
	u8  i, Sendtimes;
	u8 netisbreak;
	String_Clear(TimeSendBuf, 50);
	String_Clear(UploadTimerBuf, 50);
	
	strcpy(TimeSendBuf,"TSendheart%");	//第一次心跳
	strcat(TimeSendBuf,"FirstHeart");
	strcat(TimeSendBuf,TerminalId);
	strcat(TimeSendBuf,"%$$");
	
  strcpy(UploadTimerBuf, TimeSendBuf);
	M6312_Connecting = 1;
	Sendtimes = 0;
	netisbreak =0;
	M6312_RestartFlag = 0;
	while (Sendtimes < 3)
	{
			delay_ms(100);
			M6312_SendData(UploadTimerBuf,strlen(UploadTimerBuf));
		  Sendtimes++;
			for (i=0; i<20; i++)  //等待返回数据
			{
				if (usart1_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"K"))
					{
						delay_ms(100);
						break;
					}				
				}
				delay_ms(600);
			}
			if (i<20)  
			{
				  netisbreak = 1;
					break;
			}
	}
	if(netisbreak == 1)
	{
		HDMISendConncetOK();
		M6312_Connecting = 0;
	}
	else
	{
		M6312_RestartFlag = 0x63;
	}

}

/*******************************************************************************
* 函 数 名         : TIM_Heart_Deal
* 函数功能		     : 定时握手处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM_Heart_Deal(void)
{
	u8 i,Sendtimes;
	u8 netisbreak;
	String_Clear(TimeSendBuf, 50);
	String_Clear(UploadTimerBuf, 50);
	
	M6312_RestartFlag = 0;
	strcpy(TimeSendBuf,"TSendheart%");
	strcat(TimeSendBuf,DeviceInfo.TerminalInfo.TerminalId);
	strcat(TimeSendBuf,"%$$");
	strcat(TimeSendBuf,DeviceInfo.TerminalInfo.Fault);
	strcat(TimeSendBuf,"/");
	strcat(TimeSendBuf,DeviceInfo.TerminalInfo.SignalStrength);
	strcpy(UploadTimerBuf, TimeSendBuf);
	Sendtimes = 0;
	netisbreak = 0;
	while (Sendtimes < 3)
	{
			M6312_SendData(UploadTimerBuf, strlen(UploadTimerBuf));
		  Sendtimes++;
			for (i=0; i<20; i++)  //等待返回数据
			{
				if (usart1_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"K"))
					{
						delay_ms(100);
						break;
					}
				}
				delay_ms(600);
			}
			if (i<20)  
			{
				netisbreak = 1;
				break;
			}
	}
  if(netisbreak == 1)
	{
		M6312_SendData("HeartOK", strlen("HeartOK"));
	}
	else
	{
     M6312_RestartFlag = 0x63;
	}

}














/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
