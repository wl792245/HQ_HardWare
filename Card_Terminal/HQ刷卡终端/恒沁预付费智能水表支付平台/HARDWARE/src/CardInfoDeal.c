/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：CardInfoDeal.c
 * 项目描述 ：对华旭卡进行信息读取与处理    
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "CardInfoDeal.h"
#include "PC_Config.h"
#include "USB_Config.h"
#include "HDMI_T050.h"
#include "systick.h"
#include "hw_config.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"
#include "string.h"
#include "String_Config.h"
#include "AT24C02.h"
#include "M6312.h"
#include "stdlib.h"

volatile u8  CardCommand=0;      //卡指令

#define RCVLEN   250             //接收长度

char RecievePCBuf[RCVLEN];       //接收电脑返回的结果
char SendPCBuf[RCVLEN];          //电脑发送写卡数据

char SendServerBuf[RCVLEN];      //发送到服务器中
char RecieveServerBuf[RCVLEN];   //接收服务器返回的结果


//char ServerRecHead[18];                 //服务器取头
//char ServerRecStr[200];                 //服务器内容
HXCard4428_Info HXCard4428_RecvInfo;    //华旭4428卡接收消息结构体

#if 1
/*******************************************************************************
* 函 数 名         : HXCard_Deal
* 函数功能		     : 华旭卡通过对PC发送读卡命令获取数据进行处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
//void HXCard_Deal(void)
//{
//	u8 Sendtimes, netisbreak, pcisbreak;
//	u8 i,Char_Time;
//	u32 num[20];
//	u32 num1[20];
//	//char CardKind[10] = {0};//华旭读卡类型
//	
//	Sendtimes = 0;
//	Char_Time = 0;
//	netisbreak = 0;                     //判断当前网络线路是否正常
//	pcisbreak = 0;                      //判断当前电脑通讯是否正常
//	while (Sendtimes < 3)
//	{
//		PC_ReadCard();	//发读卡命令
//		delay_ms(100);
//		Sendtimes++;
//		for (i=0; i<20; i++)  //等待返回数据
//		{
//			if ((usart2_rcv_len > 0) || (USB_USART_RX_STA > 0)) 
//			{
//				delay_ms(341);
//				if ((NULL != strstr((const char *)usart2_rcv_buf, (const char *)"%"))||(NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"%")))
//				{
//					delay_ms(100);
//					break;
//				}				
//			}
//			delay_ms(600);
//		}
//		if (i<20)  
//		{
//			  pcisbreak = 1;                      //判断当前电脑通讯是否正常
//			  String_Clear(RecievePCBuf,RCVLEN);
//				if(usart2_rcv_len > 0)
//				{
//					PC_Usart_GetRcvData(RecievePCBuf, usart2_rcv_len);
//				}
//				if (USB_USART_RX_STA > 0)
//				{
//					PC_USB_GetRcvData(RecievePCBuf, USB_USART_RX_STA);
//				}
//				Char_Time = Char_Num(RecievePCBuf, '$');
//				if (Char_Time > 1)
//				{
//					break;
//				}
//		}
//		else
//		{
//			pcisbreak = 0;
//		}
//	}
//	if (pcisbreak == 0)
//	{
//			//读三次卡微电脑没有数据返回，系统复位
//			//等待微电脑自动关机
//			//bitCardOff = 0x68;  //发拔卡命令
//			//USB_PC_SendData("CardOff%$$");                     //USB方式
//			//USART_PC_SendData("CardOff%$$", strlen("CardOff%$$"));    //串口模式
//		  PC_SendData("CardOff%$$");
//			HDMIShowInfo("请取卡,3分钟后再插卡");
//		  PC_Close();
//			for(i=0;i<210;i++)
//			{
//				delay_ms(1024);
//			}
//		  CPU_Reset();
//	}
//	else
//	{
//		//正确接收数据
//		if (Sendtimes < 3)
//		{
//			//bitCardOff=0x68;
//			PC_SendData("CardOff%$$");
//			Interger_Clear(num,20);
//			Char_Find(num,RecievePCBuf, '%');   
//			String_Find(CardKind, RecievePCBuf, 0, num[0]);                  //取头
//			if (strcmp(CardKind,"4442")==0) //4442卡数据处理,4442卡号是14位截取后8位。
//			{
//				
//			}
//			else if (strcmp(CardKind,"4428")==0)//4428卡数据处理
//			{
//				String_Find(HXCard4428_RecvInfo.Value14428, RecievePCBuf, num[0]+1, num[1]);     //卡类型
//				String_Find(HXCard4428_RecvInfo.SByStrHX, RecievePCBuf, num[1]+1, num[2]);       //卡内容
//				if (strcmp(HXCard4428_RecvInfo.Value14428,"6")== 0)                              //4428开户卡处理
//				{
//					/*系统码|用户编号|*/
//					Interger_Clear(num1,20);
//					Char_Find(num1,HXCard4428_RecvInfo.SByStrHX, '|');   
//					String_Find(HXCard4428_RecvInfo.HX4428Sysnum, HXCard4428_RecvInfo.SByStrHX, 0, num1[0]); // 系统码
//					String_Find(HXCard4428_RecvInfo.HX4428Userid, HXCard4428_RecvInfo.SByStrHX, num1[0]+1, num1[1]); // 用户号
//					String_Clear(SendServerBuf,RCVLEN);
//					strcpy(SendServerBuf,"Hx4428USERLCID%");
//					strcat(SendServerBuf,DeviceInfo.TerminalInfo.TerminalId);
//					strcat(SendServerBuf,"%");
//					strcat(SendServerBuf,HXCard4428_RecvInfo.HX4428Userid);
//					strcat(SendServerBuf,"%");
//					strcat(SendServerBuf,HXCard4428_RecvInfo.HX4428Sysnum);
//					strcat(SendServerBuf,"%0");
//					strcat(SendServerBuf,"%$$");
//					//通过服务器查询订单
//					Sendtimes = 0;
//					Char_Time = 0;
//					while (Sendtimes < 3)
//					{
//						M6312_SendData(SendServerBuf, strlen(SendServerBuf));                       //通过M6312向服务器发数据
//						Sendtimes++;
//						for (i=0; i<20; i++)  //
//						{
//							if (usart1_rcv_len > 0) 
//							{
//								if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"%"))
//								{
//									delay_ms(100);
//									break;
//								}
//							}
//							delay_ms(341);
//						}
//						if (i < 20)
//						{
//							netisbreak = 1;                                                   //当前网络通讯正常
//							String_Clear(RecieveServerBuf, 250);
//							M6312_USART_GetRcvData(RecieveServerBuf, usart1_rcv_len);
//							Char_Time = Char_Num(RecieveServerBuf, '$');
//							if (Char_Time > 1)
//							{
//								break;
//							}
//						}
//						else
//						{
//							netisbreak = 0;
//						}
//					}
//					if (netisbreak == 0)
//					{
//						 CardCommand=0x41;	//M6312断电再上电重连网
//					}
//					else
//					{
//						if (Sendtimes < 3)
//						{
//							
//						}
//						else
//						{
//							CardCommand = 0x68;
//								//PC_SendData("CardOff%$$");
//								//HDMIShowInfo("请重刷");
//								//PC_StartPam("请重刷");
//						}
//					}
//				}
//				else if (strcmp(HXCard4428_RecvInfo.Value14428,"1")== 0)                         //4428用户卡处理
//				{
//					
//				}
//				else
//				{
//						PC_SendData("CardOff%$$");
//						PC_StartPam("未知卡,请正确插卡！");
//						HDMIShowInfo("未知卡,请正确插卡！");
//				}
//			}
//			else
//			{
//				//PC_SendData("CardOff%$$");
//				//PC_StartPam("未知卡,请正确插卡！");
//				//HDMIShowInfo("未知卡,请正确插卡！");
//				//CardCommand = 
//			}
//		}

//		else
//		{
//			//读数据不正确
//			CardCommand = 0x68;  //发拔卡命令
//			//PC_SendData("CardOff%$$");
//			//HDMIShowInfo("请重刷");
//			//PC_StartPam("请重刷");
//		}
//	}

//}
#endif
/*******************************************************************************
* 函 数 名         : SCCard_Deal
* 函数功能		     : 三川卡通过对PC发送读卡命令获取数据进行处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SCCard_Deal(void)
{
  u8 PC_ReturnValue, PC_SpitValue, M6312_ReturnValue;
  SCCard_5557PC SCCard_PCInfo;
	SCCard_Server SCCard_ServerInfo;
	
	PC_ReturnValue = 0;
	PC_SpitValue = 0;
	M6312_ReturnValue = 0;
	
	PC_RestartFlag = 0;
	M6312_RestartFlag = 0;
	//向PC发送读卡信息
	PC_ReturnValue = PC_ReadData(RecievePCBuf, 3, RCVLEN);
	//电脑通讯成功
	if (PC_ReturnValue == 1)
	{
      PC_SpitValue = SCCard_PCDataSpit(&SCCard_PCInfo, SendServerBuf,RecievePCBuf);
		  //电脑返回数据正确
			if (PC_SpitValue == 1)
			{
				//与服务器进行通讯
				M6312_ReturnValue = M6312_UploadData(RecieveServerBuf, SendServerBuf, "%$$", 3,RCVLEN);
				//服务器通讯成功
				if (M6312_ReturnValue == 1)
				{
					SCCard_ServerDataSpit(&SCCard_ServerInfo, SCCard_PCInfo, SendServerBuf, RecievePCBuf,RecieveServerBuf, SendPCBuf);
				}
				else
				{
					M6312_RestartFlag = 1;  //M6312重启
				}
			}
			else if (PC_SpitValue == 0)
			{
					HDMIShowInfo("未知卡,请正确插卡！");
				  delay_ms(10);
		     	PC_StartPam("未知卡,请正确插卡！");
			}
			else if(PC_SpitValue == 2)
			{
				HDMIShowMenuInfo(&DeviceInfo);        //主界面显示
				return ;
			}				
	}
	//电脑通讯失败
	else
	{
		  PC_StartPam("请取卡,3分钟后再插卡");
			delay_ms(300);
			HDMIShowInfo("请取卡,3分钟后再插卡");
		  PC_RestartFlag = 1;   //电脑重启
	}
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/

