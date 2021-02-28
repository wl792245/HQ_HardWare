/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：SCCard_Config.c
 * 项目描述 ：三川卡模块功能定义     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-2-3
**********************************************************************************/
#include "SCCard_Config.h"
#include "stm32f10x_exti.h"
#include "String_Config.h"
#include "string.h"
#include "HDMI_T050.h"
#include "systick.h"
#include "PC_Config.h"
#include "AT24C02.h"
#include "M6312.h"
volatile uint32_t   SCCardIsInter=0; //三川5557卡中断标志位
/*******************************************************************************
* 函 数 名         : SCCard_Init
* 函数功能		     : 三川卡模块初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SCCard_Init(u16 m_irq, u16 s_irq)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		//配置gpio，三川插卡中断,PB5
		GPIO_InitStructure.GPIO_Pin=SCCARD_INIT_CSW;	 //选择你要设置的IO口(pin_5)
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	  //设置上拉输入模式
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
		GPIO_Init(SCCARD_INIT_Port,&GPIO_InitStructure); /* 初始化GPIO */	
		/*将GPIO管脚与外部中断线连接*/
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);//选择GPIO管脚用作外部中断线路
		/*配置EXTI*/
		EXTI_InitStructure.EXTI_Line=SCCARD_Line;
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure); 
		
		/* 设置NVIC参数 */	
		NVIC_InitStructure.NVIC_IRQChannel = SCCARD_INIT_IRQn;		//打开EXTI1的全局中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //抢占优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //响应优先级为2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //使能
		NVIC_Init(&NVIC_InitStructure);	
}
/*******************************************************************************
* 函 数 名         : SCCard_PCDataSpit
* 函数功能		     : 三川卡电脑数据分解
* 输    入         : SCCardPC:写入电脑返回的数据  pdst：写入服务器传输数据 psrc：分解电脑收到的数据
* 输    出         : 无
*******************************************************************************/
u8 SCCard_PCDataSpit(PSCCard_5557PC SCCardPC,char *pdst_ServerSend,char *psrc_PCRecv)
{
	  u32 num[20];
		Interger_Clear(num,20);
		String_Clear(SCCardPC->PCRecHead, 2);
		String_Clear(SCCardPC->PCRecStr,200);
		Char_Find(num,psrc_PCRecv, '%');   
		String_Find(SCCardPC->PCRecHead, psrc_PCRecv, 0, num[0]);                  //取头
	  if (strcmp(SCCardPC->PCRecHead, "1") == 0)                                  //dll返回读卡值
		{
			String_Find(SCCardPC->PCRecStr, psrc_PCRecv, num[0]+1, num[1]);            // 卡内容
			Interger_Clear(num,20);
			Char_Find(num,SCCardPC->PCRecStr, '|');
			String_Find(SCCardPC->iCardTypeSC, SCCardPC->PCRecStr, 0, num[0]);                // 三川5557卡类型
			String_Find(SCCardPC->iManageCodeSC, SCCardPC->PCRecStr, num[0]+1, num[1]);       // 三川5557卡系统维护码
			String_Find(SCCardPC->iKeySC, SCCardPC->PCRecStr, num[1]+1, num[2]);              // 三川5557卡密钥
			String_Find(SCCardPC->CardIdSC, SCCardPC->PCRecStr, num[2]+1, num[3]);            // 三川5557卡卡号
			String_Find(SCCardPC->BiaoIdSC, SCCardPC->PCRecStr, num[3]+1, num[4]);            // 三川5557卡表号
			String_Find(SCCardPC->BuyNumSC, SCCardPC->PCRecStr, num[4]+1, num[5]);            // 三川5557卡充值序号
			String_Find(SCCardPC->CardMonenySC, SCCardPC->PCRecStr, num[5]+1, num[6]);        // 三川5557卡总充值金额
			String_Find(SCCardPC->BuyAllNumSC, SCCardPC->PCRecStr, num[6]+1, num[7]);         // 三川5557卡总充值总序号
			String_Find(SCCardPC->UseWaterSC, SCCardPC->PCRecStr, num[7]+1, num[8]);          // 三川5557卡用水量（0.1吨）
			String_Find(SCCardPC->ZhuangTaiMaSC, SCCardPC->PCRecStr, num[8]+1, num[9]);       // 三川5557卡状态码
				//新购水未圈存，请先刷表
			if ((strcmp(SCCardPC->iCardTypeSC, "288") == 0)||(strcmp(SCCardPC->iCardTypeSC, "298") == 0))
			{
				return 2;
			}
			else
			{
				String_Clear(pdst_ServerSend,250);
				strcpy(pdst_ServerSend,"SANC5557PCID%");
				strcat(pdst_ServerSend,DeviceInfo.TerminalInfo.TerminalId);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->CardIdSC);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->iManageCodeSC);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->CardMonenySC);
				strcat(pdst_ServerSend,"%$$");
				return 1;
			}
		}
		else
		{
			  return 0;
		}
		
}
/*******************************************************************************
* 函 数 名         : SCCard_ServerDataSpit
* 函数功能		     : 三川卡服务器数据分解
* 输    入         : SCCardServer:写入服务器返回的数据  SCCardPC:传入电脑返回的数据 pdst_ServerSend：服务器发送数据 pdst_PCRec: 电脑接收数据， psrc：分解数据源 writecard:写入写卡数据
* 输    出         : 无
*******************************************************************************/
void SCCard_ServerDataSpit(PSCCard_Server SCCardServer,const SCCard_5557PC SCCardPC, char *pdst_ServerSend, char *pdst_PCRec, char *psrc_Server, char * PCSend)
{
	  u32 num[20];
	  char  PC_ReturnHead[3];
	  CardType_Info CardType = SC5557;
	  u8 PC_ReturnValue, M6312_ReturnValue;
	  
	  UserInfoShow  UserInfo;
	  PC_ReturnValue = 0;
	  M6312_ReturnValue = 0;
	  Get_Head(SCCardServer->ServerRecHead,psrc_Server, '%', num, 20);
	  //头判断
    if(strcmp(SCCardServer->ServerRecHead,"SC5557USERP")==0)	                //三川5557新增订单处理
		{
			String_Find(SCCardServer->SCID, psrc_Server, num[0]+1, num[1]);                // 取设备号
			String_Find(SCCardServer->SC5557CardID, psrc_Server, num[1]+1, num[2]);        // 取卡号
			String_Find(SCCardServer->SC5557OrderId, psrc_Server, num[2]+1, num[3]);       // 取订单号
			String_Find(SCCardServer->iPrice1SC, psrc_Server, num[3]+1, num[4]);           // 取一价
			String_Find(SCCardServer->iPrice2SC, psrc_Server, num[4]+1, num[5]);           // 取二价
			String_Find(SCCardServer->iPrice3SC, psrc_Server, num[5]+1, num[6]);           // 取三价
			String_Find(SCCardServer->iTon1SC, psrc_Server, num[6]+1, num[7]);             // 取一量
			String_Find(SCCardServer->iTon2SC, psrc_Server, num[7]+1, num[8]);             // 取二量
			String_Find(SCCardServer->BuyMonenySC, psrc_Server, num[8]+1, num[9]);         // 取充值金额
			
		  String_Clear(PCSend, 250);
			strcpy(PCSend,SCCardPC.BiaoIdSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->BuyMonenySC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardPC.BuyNumSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardPC.BuyAllNumSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iTon1SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iTon2SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice1SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice2SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice3SC);
			strcat(PCSend,"|");
			//写卡
			PC_ReturnValue = PC_WirteData(pdst_PCRec,PCSend,3,250,CardType);
			//写卡成功，接收到PC返回的数据
			if (PC_ReturnValue == 1)
			{
				Get_Head(PC_ReturnHead ,pdst_PCRec, '%', num, 20);
				//写卡成功
				if (strcmp(PC_ReturnHead, "1") == 0)
				{
					SCCard_ServerDataCombine(pdst_ServerSend, *SCCardServer, PC_ReturnHead,1);
					M6312_ReturnValue = M6312_UploadData(psrc_Server, pdst_ServerSend, "OK", 3, 250);
					if (M6312_ReturnValue == 1)
					{
						if ((strcmp(psrc_Server, "updateorderstateOK2")==0) || (strstr(psrc_Server, "pdateorderstateOK2")!= NULL))
						{
							//充值成功
							HDMIShowRechargeSucess(SCCardServer->BuyMonenySC, 0);
							Result_StartPam(SCCardServer->BuyMonenySC, 0);
						}
						else if (strcmp(psrc_Server,"updateorderstateOK1")==0)
						{
							HDMIShowInfo("交易关闭，请营业厅办理");
							PC_StartPam("交易关闭，请营业厅办理");
						}
					}						
				}
				else 
				{
					HDMIShowInfo("充值失败，请重新刷卡");
					delay_ms(10);
					PC_StartPam("充值失败，请重新刷卡");
					SCCard_ServerDataCombine(pdst_ServerSend, *SCCardServer, PC_ReturnHead,0);
					M6312_SendData(pdst_ServerSend, strlen(pdst_ServerSend));
				}
			}
			//未接收到PC返回的数据
			else
			{
				HDMIShowInfo("充值失败，请重新刷卡");
				PC_StartPam("充值失败，请重新刷卡");
			}
		}
		else if (strcmp(SCCardServer->ServerRecHead,"SC5557USERPcheck")==0) //三川5557查询卡处理SC5557USERPcheck
		{
			String_Find(SCCardServer->SCID, psrc_Server, num[0]+1, num[1]);                // 取设备号
			String_Find(SCCardServer->SC5557CardID, psrc_Server, num[1]+1, num[2]);        // 取卡号
			String_Find(SCCardServer->SC5557UserID, psrc_Server, num[2]+1, num[3]);        // 取用户号
			String_Find(SCCardServer->SCAdress, psrc_Server, num[3]+1, num[4]);            //取用户地址
			String_Find(SCCardServer->SCXingzhi, psrc_Server, num[4]+1, num[5]);           //取用水性质
			String_Find(SCCardServer->SCTotalamount, psrc_Server, num[5]+1, num[6]);       //取用户水表中水的总量
			String_Find(SCCardServer->SCLastAllBuyMount, psrc_Server, num[6]+1, num[7]);   //取用户当年已购水次数
			UserInfo.UserInfo.userId = SCCardServer->SC5557UserID;
			UserInfo.UserInfo.userAdress = SCCardServer->SCAdress;
			UserInfo.WaterInfo.waterProperties = SCCardServer->SCXingzhi;
			UserInfo.WaterInfo.totalAmount = SCCardServer->SCTotalamount;
			UserInfo.WaterInfo.LastAllBuyMount = SCCardServer->SCLastAllBuyMount;
			UserInfo.WaterInfo.Unit = 1;
			if (strlen(SCCardServer->SCAdress) > 18)
			{
				UserInfo.islongthan12 = 1;
			}
			else
			{
				UserInfo.islongthan12 = 0;
			}
			HDMIShowUserDetail(UserInfo);
			PC_StartPam("微信缴费后请将卡取出，重新插入完成写卡操作");
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr0")==0)//未知用户，请营业厅办理
		{
			HDMIShowInfo("未知用户，请营业厅办理");
			PC_StartPam("未知用户，请营业厅办理");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557Perr1")==0)//请微信支付后再刷卡
		{
			HDMIShowInfo("请微信支付后再刷卡");
			PC_StartPam("请微信支付后再刷卡");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr2")==0)//请微信支付后再刷卡
		{
			HDMIShowInfo("请微信支付后再刷卡");
			PC_StartPam("请微信支付后再刷卡");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557err3")==0)//无效卡，请营业厅办理
		{
			HDMIShowInfo("无效卡，请营业厅办理");
			PC_StartPam("无效卡，请营业厅办理");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr12")==0)//交易关闭，请营业厅办理
		{
			HDMIShowInfo("交易关闭，请营业厅办理");
			PC_StartPam("交易关闭，请营业厅办理");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"WebserviceErro")==0)
		{
			HDMIShowInfo("网络服务未开启");
			PC_StartPam("网络服务未开启");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"WebNoAnswer")==0)
		{
			HDMIShowInfo("网络无响应");
			PC_StartPam("网络无响应");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557PCID")==0)
		{
			
		}
		else
		{
		}

}
/*******************************************************************************
* 函 数 名         : SCCard_ServerDataCombine
* 函数功能		     : 三川卡服务器数据组合
* 输    入         : pdst_ServerSend:服务器发送数据  SCCardServer:服务器接收数据 WriteCardSuccess:写卡成功类型 isSuccess:是否写卡成功
* 输    出         : 无
*******************************************************************************/
void SCCard_ServerDataCombine(char *pdst_ServerSend, const SCCard_Server SCCardServer, char * WriteCardSuccess, int isSuccess)
{
	String_Clear(pdst_ServerSend, 250);
	strcpy(pdst_ServerSend,"SANC5557Result%");
	strcat(pdst_ServerSend,SCCardServer.SCID);
	strcat(pdst_ServerSend,"%");
	strcat(pdst_ServerSend,SCCardServer.SC5557CardID);
	strcat(pdst_ServerSend,"%");
	strcat(pdst_ServerSend,SCCardServer.SC5557OrderId);	
	if (isSuccess == 1)
	{
		strcat(pdst_ServerSend,"%Success%$$");
	}
	else if (isSuccess == 0)
	{
		strcat(pdst_ServerSend,"%Fail%$$");
	}
	strcat(pdst_ServerSend,WriteCardSuccess);
}





/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
