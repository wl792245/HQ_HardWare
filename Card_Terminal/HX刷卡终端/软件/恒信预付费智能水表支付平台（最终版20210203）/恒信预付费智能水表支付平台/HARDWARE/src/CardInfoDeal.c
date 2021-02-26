/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：CardInfoDeal.c
 * 项目描述 ：对24卡、57卡、42卡进行信息读取与处理    
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "CardInfoDeal.h"
#include "stm32f10x_exti.h"
#include "string.h"


#include "M6311.h"
#include "systick.h"
#include "AT24C02.h"
#include "XFS.h"
#include "HDMI_T050.h"
#include "utils.h"
#include "String_Get.h"

volatile uint32_t   CardIsInter=0; //42.24卡中断标志位

UserInfoShow        UserInfoDisplay;   //用户及用水信息结构体
Card_DealSend       Card_SendInfo;     //卡发送信息结构体
Card_DealRecv       Card_RecvInfo;     //卡接收信息结构体

char UploadServerBuf[200];    //上传服务器数据
char RecieveServerBuf[200];   //接收服务器返回的结果
char voicesucc[30];           // 用于播放成功
u32 num[20];                     //查找分隔符的个数

int islongthan12 = 0;                  //字符长度超过12
u32 Char_Time = 0;                     //查找字符重复次数
u8 Sendtimes = 0;                      //上传服务器的数据次数
u8 netisbreak = 0;                     //判断当前网络线路是否通
/*******************************************************************************
* 函 数 名         : Card_PowerInit
* 函数功能		     : 给24卡.42卡电源引脚初始化  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void Card_PowerInit(void)	  
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); /* 开启GPIO时钟 */
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=Card_POWER;				//选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		//设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;		//设置传输速率
	GPIO_Init(Card_GPIO,&GPIO_InitStructure); /* 初始化GPIO */	
}
/*******************************************************************************
* 函 数 名         : Card_Inter
* 函数功能		     : 24卡.42卡中断配置
* 输    入         : m_irq：抢占优先级（1），s_irq：响应优先级（0）
* 输    出         : 无
*******************************************************************************/
static void Card_Inter(u16 m_irq, u16 s_irq)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=Card_ICSW ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(Card_GPIO,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);//选择GPIO管脚用作外部中断线路
	//此处一定要记住给端口管脚加上中断外部线路
	/* 设置外部中断的模式 */ 
	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* 设置NVIC参数 */	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
	NVIC_InitStructure.NVIC_IRQChannel = Card_IRQn; 	//打开EXTI2的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //使能
	NVIC_Init(&NVIC_InitStructure); 
}	
/*******************************************************************************
* 函 数 名         : Card_Init
* 函数功能		     : 24卡.42卡初始化 
* 输    入         : m_irq：抢占优先级（1），s_irq：响应优先级（0）
* 输    出         : 无
*******************************************************************************/
void Card_Init(u16 m_irq, u16 s_irq)
{
	I2C_INIT();
	Card_PowerInit();
	Card_Inter( m_irq, s_irq);
}
/*******************************************************************************
* 函 数 名         : CardCIDCopy24C02
* 函数功能		     : 将24卡的机器号IICID复制到开发板上的24c02  5-37 32位
* 输    入         : 无
* 输    出         : 0:复制成功，1：复制失败
*******************************************************************************/
u8 CopyCIDTo24C02(void)
{
	char ConfigurationCardInfo[32];
	unsigned int Num[5] = {0};
	Card24_Read(5,ConfigurationCardInfo,32);
	Char_Find(Num, ConfigurationCardInfo, '%');
	delay_ms(100);
	if((Num[0] == 10) &&(Num[1] == 31))	
	{
		while(AT24C02_Write(5,ConfigurationCardInfo,32))
		{
			return 1;
		}
		return 0;
	}
	else
	{
		return 1;
	}
}
/*******************************************************************************
* 函 数 名         : CopyScrrenvTo24C02
* 函数功能		     : 将24卡的屏幕版本号复制到开发板上的24c02  37-42 5位
* 输    入         : 无
* 输    出         : 0:复制成功，1：复制失败
*******************************************************************************/
u8 CopyScrrenvTo24C02(void)
{
	char ConfigurationCardInfo[5];
	Card24_Read(37,ConfigurationCardInfo,5);
	delay_ms(100);
	while(AT24C02_Write(37,ConfigurationCardInfo,5))
	{
			return 1;
	}
	return 0;
}
/*******************************************************************************
* 函 数 名         : CopyTerminalVTo24C02
* 函数功能		     : 将24卡的设备版本号复制到开发板上的24c02 42-47 5位
* 输    入         : 无
* 输    出         : 0:复制成功，1：复制失败
*******************************************************************************/
u8 CopyTerminalVTo24C02(void)
{
	char ConfigurationCardInfo[5];
	Card24_Read(42,ConfigurationCardInfo,5);
	delay_ms(100);
	while (AT24C02_Write(42,ConfigurationCardInfo,5))
	{
		return 1;
	}
	return 0;

}
/*******************************************************************************
* 函 数 名         : CopyIPPortTo24C02
* 函数功能		     : 将24卡的服务器地址端口号复制到开发板上的24c02 47-69 22位
* 输    入         : 无
* 输    出         : 0:复制成功，1：复制失败
*******************************************************************************/
u8 CopyIPPortTo24C02(void)
{
	char ConfigurationCardInfo[22];
	Card24_Read(47,ConfigurationCardInfo,22);
	delay_ms(100);
	while (AT24C02_Write(47,ConfigurationCardInfo,22))
	{
		return 1;
	}
	return 0;

}
/*******************************************************************************
* 函 数 名         : CopyQRcodeTo24C02
* 函数功能		     : 将24卡的二维码复制到开发板上的24c02 69-90 21位
* 输    入         : 无
* 输    出         : 0:复制成功，1：复制失败
*******************************************************************************/
u8 CopyQRcodeTo24C02(void)
{
	char  ConfigurationCardInfo[21];
	Card24_Read(69,ConfigurationCardInfo,21);
	delay_ms(100);
	while(AT24C02_Write(69,ConfigurationCardInfo,21))
	{
		return 1;
	}
	return 0;
}
/*******************************************************************************
* 函 数 名         : Card24_PDeal
* 函数功能		     : 24卡预付费处理函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card24_PDeal(Card24_PreInfo *Info24p)
{
		Card24_PreInfo      SInfo24pre;         //24卡预付费信息结构体
	
	  u8  RecvState = 0;

    islongthan12 = 0;                  //字符长度超过12
		netisbreak = 0;                     //判断当前网络线路是否通
		
		Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info24p->cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
		HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
		Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info24p->areacode, 6);//将3字节区域号拆分成6个半字节数组便于转换成字符
		HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 6);//将4个半字节数组区域号转成字符数组
		
		strcpy(Card_SendInfo.TSendCID,"TSend24PCID%");                                    //24预付费卡CID
		strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId);               //24预付费卡机器号
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //24预付费卡卡号
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //24预付费卡区域号
		strcat(Card_SendInfo.TSendCID,"%$$");
		strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
		netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
		if(netisbreak==0)     //通讯失败
		{
			XFS_Fail_Play("24预付费通讯失败，重启中");
			CPU_Reset();
		}
		else if (netisbreak == 1)                //通讯正常
		{
					Interger_Clear(num,20);
					Char_Find(num,RecieveServerBuf, '%');                                          //获取百分号的位置
					String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //取头
					if (strcmp(Card_RecvInfo.head,"SSend24P")==0)                                  //判断为24预付费卡	
					{
							String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
							String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
							String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //取订单号
							String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //取充值量
							String_Find(Card_RecvInfo.times, RecieveServerBuf, num[4]+1, num[5]);          //取次数
							String_Find(Card_RecvInfo.security, RecieveServerBuf, num[5]+1, num[6]);        //取保密位
							String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[6]+1, num[7]);  //取本次充值量便于语音播报
							/*将服务器下传数据保存起来*/
							strcpy(Card_SendInfo.TSendCID,"TSend24PResult%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
							strcat(Card_SendInfo.TSendCID,"%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
							strcat(Card_SendInfo.TSendCID,"%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
						
							Card24_PreReadCID(&SInfo24pre);			  //读24预付费卡
							Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo24pre.cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
							HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
							if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)//判断服务器返回的卡号是否和当前卡一致
							{
								strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
								strcpy((char *)Card_SendInfo.Card_CharInfo.strtimes, Card_RecvInfo.times);
								strcpy((char *)Card_SendInfo.Card_CharInfo.strsecurity, Card_RecvInfo.security);
								StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,2);//将充值量字符串转换为十六进制
								StrToHex(Card_SendInfo.Card_HexInfo.hextime, Card_SendInfo.Card_CharInfo.strtimes,1);//将次数字符串转换为十六进制
								StrToHex(Card_SendInfo.Card_HexInfo.hexsecurity, Card_SendInfo.Card_CharInfo.strsecurity,2);//将保密位字符串转换成十六进制字节
								Card_SendInfo.Card_Flag = Card24_PreWriteSaleAmount(Card_SendInfo.Card_HexInfo.hexbuyamount,Card_SendInfo.Card_HexInfo.hexsecurity, Card_SendInfo.Card_HexInfo.hextime[0]);//写卡
								if (Card_SendInfo.Card_Flag == 1)
								{
									strcat(Card_SendInfo.TSendCID,"%Success%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
									if (RecvState == 0)
									{
											XFS_Fail_Play("通信失败，请重刷");     //网络正常，重新刷卡
									}
									else if (RecvState == 1)
									{
											if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
											{
												String_Clear(voicesucc, 30);
												strcpy(voicesucc,"您已成功充值");
												strcat(voicesucc, Card_RecvInfo.pricemountstr);
												strcat(voicesucc,"吨");
												XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
											}
											else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
											{
												XFS_Fail_Play("交易未受理，请重刷");
											}
									}
								}
								else
								{
									XFS_Fail_Play("充值失败,请重刷"); //语音播报，屏幕显示
									strcat(Card_SendInfo.TSendCID,"%Fail%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
								}														
							}
							else
							{
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
								XFS_Fail_Play("请重刷");     //网络正常，重新刷卡
							}
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Pcheck")==0)//判断为24预付费卡check
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
						String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //取用户号
						String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //取用户地址
						if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
						else islongthan12 = 0;
						String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //取用水性质
						String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //取用户水表中水的总量
						String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //取用户当年已购水量
						UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
						UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
						UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
						UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
						UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
						UserInfoDisplay.islongthan12 = islongthan12;
						UserInfoDisplay.WaterInfo.Unit = 1;
						HDMIShowUserDetail(UserInfoDisplay);
						XFS_Only_Play("微信缴费后请将卡取出，重新插入完成写卡操作");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr0")==0)
					{
						XFS_Fail_Play("未知用户，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr1")==0)
					{
						XFS_Fail_Play("请微信支付后再刷卡");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr3")==0)
					{
						XFS_Fail_Play("无效卡，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr12")==0)
					{
						XFS_Fail_Play("交易关闭，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
					{
						XFS_Fail_Play("网络服务未开启");
					}
					else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
					{
						XFS_Fail_Play("网络无响应");
					}
					else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
					{
							char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
							AT24C02_Write(100,clear,5);	//清空缓存中的头
					}
					else
					{
						
					}
				
		}

}
/*******************************************************************************
* 函 数 名         : Card24_LDeal
* 函数功能		     : 24卡阶梯处理函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card24_LDeal(Card24_LadderInfo *Info24l)
{
	Card24_LadderInfo   SInfo24ladder;      //24卡阶梯信息结构体
	u8 price[8]= {'\0'};
	u8 amount[6]= {'\0'};

  u8  RecvState = 0;
		
	islongthan12 = 0;                  //字符长度超过12
  netisbreak = 0;                     //判断当前网络线路是否通
	

	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info24l->cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info24l->areacode, 6);//将3字节区域号拆分成6个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 6);//将6个半字节数组区域号转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info24l->watersalesamount, 8);//将4字节售水量拆分成8个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 8);//将8个半字节数组售水量转成字符数组
	
	strcpy(Card_SendInfo.TSendCID,"TSend24LCID%");                                    //24阶梯卡CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId);               //24阶梯卡机器号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //24阶梯卡卡号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //24阶梯卡区域号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);//24阶梯卡售水量
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	if(netisbreak==0)     //通讯失败
	{
		XFS_Fail_Play("24阶梯通讯失败，重启中");
		CPU_Reset();
	}
	else                 //通讯正常
	{
			Interger_Clear(num,20);
			Char_Find(num,RecieveServerBuf, '%');                                          //获取百分号的位置
			String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //取头
		  if (strcmp(Card_RecvInfo.head,"SSend24L")==0)                                  //判断为24阶梯卡	
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
				String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //取订单号
				String_Find(Card_RecvInfo.oneprice, RecieveServerBuf, num[3]+1, num[4]);        //取一价
				String_Find(Card_RecvInfo.twoprice, RecieveServerBuf, num[4]+1, num[5]);        //取二价
				String_Find(Card_RecvInfo.threeprice, RecieveServerBuf, num[5]+1, num[6]);      //取三价
				String_Find(Card_RecvInfo.fourprice, RecieveServerBuf, num[6]+1, num[7]);       //取四价
				String_Find(Card_RecvInfo.oneamount, RecieveServerBuf, num[7]+1, num[8]);       //取一量
				String_Find(Card_RecvInfo.twoamount, RecieveServerBuf, num[8]+1, num[9]);       //取二量
				String_Find(Card_RecvInfo.threeamount, RecieveServerBuf, num[9]+1, num[10]);    //取三量
				String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[10]+1, num[11]);       //取充值量
				String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[11]+1, num[12]);  //取本次充值量便于语音播报
				/*将服务器下传数据保存起来*/
				strcpy(Card_SendInfo.TSendCID,"TSend24LResult%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
				
				Card24_LadderReadCID(&SInfo24ladder);
				Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo24ladder.cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
				HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
				if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)//判断服务器返回的卡号是否和当前卡一致
				{
					strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneprice, Card_RecvInfo.oneprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoprice, Card_RecvInfo.twoprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeprice, Card_RecvInfo.threeprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strfourprice, Card_RecvInfo.fourprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneamount, Card_RecvInfo.oneamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoamount, Card_RecvInfo.twoamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeamount, Card_RecvInfo.threeamount);
					
					StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,4);//将充值量字符串转换为十六进制
					StrToHex(Card_SendInfo.Card_HexInfo.hexoneprice, Card_SendInfo.Card_CharInfo.stroneprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hextwoprice, Card_SendInfo.Card_CharInfo.strtwoprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexthreeprice, Card_SendInfo.Card_CharInfo.strthreeprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexfourprice, Card_SendInfo.Card_CharInfo.strfourprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexoneamount, Card_SendInfo.Card_CharInfo.stroneamount, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hextwoamount, Card_SendInfo.Card_CharInfo.strtwoamount, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexthreeamount, Card_SendInfo.Card_CharInfo.strthreeamount, 2);
					price[0] = Card_SendInfo.Card_HexInfo.hexoneprice[0];
					price[1] = Card_SendInfo.Card_HexInfo.hexoneprice[1];
					price[2] = Card_SendInfo.Card_HexInfo.hextwoprice[0];
					price[3] = Card_SendInfo.Card_HexInfo.hextwoprice[1];
					price[4] = Card_SendInfo.Card_HexInfo.hexthreeprice[0];
					price[5] = Card_SendInfo.Card_HexInfo.hexthreeprice[1];
					price[6] = Card_SendInfo.Card_HexInfo.hexfourprice[0];
					price[7] = Card_SendInfo.Card_HexInfo.hexfourprice[1];
					amount[0] = Card_SendInfo.Card_HexInfo.hexoneamount[0];
					amount[1] = Card_SendInfo.Card_HexInfo.hexoneamount[1];
					amount[2] = Card_SendInfo.Card_HexInfo.hextwoamount[0];
					amount[3] = Card_SendInfo.Card_HexInfo.hextwoamount[1];
					amount[4] = Card_SendInfo.Card_HexInfo.hexthreeamount[0];
					amount[5] = Card_SendInfo.Card_HexInfo.hexthreeamount[1];
					
					Card_SendInfo.Card_Flag = Card24_LadderWrite(Card_SendInfo.Card_HexInfo.hexbuyamount, price, amount);//写卡
					if (Card_SendInfo.Card_Flag == 1)
					{
						strcat(Card_SendInfo.TSendCID,"%Success%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
            if (RecvState == 0)
						{
							XFS_Fail_Play("通信失败，请重刷");
						}
						else if (RecvState == 1)
						{
							if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
							{
								String_Clear(voicesucc, 30);
								strcpy(voicesucc,"您已成功充值");
								strcat(voicesucc, Card_RecvInfo.pricemountstr);
								strcat(voicesucc,"元");
								XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 0);
							}
							else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
							{
								XFS_Fail_Play("交易未受理，请重刷");
							}
						}
					}
					else
					{
						XFS_Fail_Play("充值失败,请重刷"); //语音播报，屏幕显示
						strcat(Card_SendInfo.TSendCID,"%Fail%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
					}									
				}
				else
				{
					strcat(Card_SendInfo.TSendCID,"%Fail%$$");
					strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
					M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
				  XFS_Fail_Play("请重刷");
				}
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lcheck")==0)                          //判断为24阶梯卡check
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
				String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //取用户号
				String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //取用户地址
				if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
				else islongthan12 = 0;
				String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //取用水性质
				String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //取用户水表中水的总量
				String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //取用户当年已购水量
				UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
				UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
				UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
				UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
				UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
				UserInfoDisplay.islongthan12 = islongthan12;
				UserInfoDisplay.WaterInfo.Unit = 0;
				HDMIShowUserDetail(UserInfoDisplay);
				XFS_Only_Play("微信缴费后请将卡取出，重新插入完成写卡操作");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr0")==0)
			{
				XFS_Fail_Play("未知用户，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr1")==0)
			{
				XFS_Fail_Play("请微信支付后再刷卡");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr3")==0)
			{
				XFS_Fail_Play("无效卡，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr12")==0)
			{
				XFS_Fail_Play("交易关闭，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
			{
				XFS_Fail_Play("网络服务未开启");
			}
			else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
			{
				XFS_Fail_Play("网络无响应");
			}
			else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
			{
					char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
					AT24C02_Write(100,clear,5);	//清空缓存中的头
			}
			else
			{
				
			}
	}
}
/*******************************************************************************
* 函 数 名         : Card42_PDeal
* 函数功能		     : 42卡预付费处理函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card42_PDeal(Card42_PreInfo *Info42p)
{
		Card42_PreInfo      SInfo42pre;         //42卡预付费信息结构体
	

		u8  RecvState = 0;
	
		islongthan12 = 0;                  //字符长度超过12
	  netisbreak = 0;                     //判断当前网络线路是否通
		
		Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info42p->cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
		HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
		Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info42p->areacode, 4);//将2字节区域号拆分成4个半字节数组便于转换成字符
		HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 4);//将4个半字节数组区域号转成字符数组
		Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info42p->watersalesamount, 6);//将3字节售水量拆分成6个半字节数组便于转换成字符
		HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 6);//将6个半字节数组售水量转成字符数组
		
		strcpy(Card_SendInfo.TSendCID,"TSend4442CID%");  //42卡预付费CID
		strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //42卡预付费机器号
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //42卡卡号
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //42卡区域号
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);//42卡售水量
		strcat(Card_SendInfo.TSendCID,"%$$");
		strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
		netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
		if(netisbreak==0)     //通讯失败
		{
			XFS_Fail_Play("42预付费通讯失败，重启中");
			CPU_Reset();
		}
		else                 //通讯成功
		{
				  Interger_Clear(num,20);
					Char_Find(num,RecieveServerBuf, '%');                                          //获取百分号的位置
					String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //取头
					if(strcmp(Card_RecvInfo.head,"SSend4442")==0)                                  //判断为4442卡
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
						String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //取订单号
						String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //取充值量
						String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[4]+1, num[5]);  //取本次充值量便于语音播报
						strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
						/*将服务器下传数据保存起来*/
						strcpy(Card_SendInfo.TSendCID,"TSend4442Result%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
						strcat(Card_SendInfo.TSendCID,"%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
						strcat(Card_SendInfo.TSendCID,"%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
						
						Card42_PreReadCID(&SInfo42pre);			  //再次读4442卡
						
						Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo42pre.cardId, 8); //将4字节卡号拆分成8个半字节数组便于转换成字符
						HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//将8个半字节数组卡号转成字符数组
						if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
						{
							StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,3);//将充值量字符串转换为十六进制
							Card_SendInfo.Card_Flag = Card42_Writesalesamount(Card_SendInfo.Card_HexInfo.hexbuyamount); //将充值量写卡
							if (Card_SendInfo.Card_Flag == 1)
							{
								strcat(Card_SendInfo.TSendCID,"%Success%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
								if (RecvState == 0)
								{
									XFS_Fail_Play("通信失败，请重刷");     //网络正常，重新刷卡
								}
								else if (RecvState == 1)
								{
									if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
									{
										String_Clear(voicesucc, 30);
										strcpy(voicesucc,"您已成功充值");
										strcat(voicesucc, Card_RecvInfo.pricemountstr);
										strcat(voicesucc,"吨");
										XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
									}
									else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
									{
										XFS_Fail_Play("交易未受理，请重刷");
									}
								}
								
							}
							else
							{
								XFS_Fail_Play("充值失败,请重刷"); //语音播报，屏幕显示
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
							}
						}
						else
						{
							strcat(Card_SendInfo.TSendCID,"%Fail%$$");
							strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
							M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
							XFS_Fail_Play("请重刷");     //网络正常，重新刷卡
						}
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442check")==0)//判断为4442卡check
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
						String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //取用户号
						String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //取用户地址
						if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
						else islongthan12 = 0;
						String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //取用水性质
						String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //取用户水表中水的总量
						String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //取用户当年已购水量
						UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
						UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
						UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
						UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
						UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
						UserInfoDisplay.islongthan12 = islongthan12;
						UserInfoDisplay.WaterInfo.Unit = 1;
						HDMIShowUserDetail(UserInfoDisplay);
						XFS_Only_Play("微信缴费后请将卡取出，重新插入完成写卡操作");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err0")==0)
					{
						XFS_Fail_Play("未知用户，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err1")==0)
					{
						XFS_Fail_Play("请微信支付后再刷卡");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err3")==0)
					{
						XFS_Fail_Play("无效卡，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err12")==0)
					{
						XFS_Fail_Play("交易关闭，请营业厅办理");
					}
					else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
					{
						XFS_Fail_Play("网络服务未开启");
					}
					else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
					{
						XFS_Fail_Play("网络无响应");
					}
					else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
					{
							char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
							AT24C02_Write(100,clear,5);	//清空缓存中的头
					}
					else
					{
						
					}
		}
		
		
}
/*******************************************************************************
* 函 数 名         : Card57_PDeal
* 函数功能		     : 57卡预付费处理函数
* 输    入         : Info57p:57卡预付费结构体
* 输    出         : 无
*******************************************************************************/
void Card57_PDeal(Card57_PreInfo * Info57p)
{
	Card57_PreInfo      SInfo57pre;

	u8 k;
	u8  RecvState = 0;
	islongthan12 = 0;                  //字符长度超过12
	netisbreak = 0;                     //判断当前网络线路是否通

	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info57p->cardId, 10); //将5字节卡号拆分成10个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//将10个半字节数组卡号转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info57p->watersalesamount, 4);//将2字节售水量拆分成4个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 4);//将4个半字节数组售水量转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hextotal, Info57p->total, 4);//将2字节售水总量拆分成4个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.strtotal, Card_SendInfo.Card_HexInfo.hextotal, 4);//将4个半字节数组售水总量转成字符数组
	
	strcpy(Card_SendInfo.TSendCID,"TSend5557PCID%");  //57卡预付费CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //57卡预付费机器号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //57卡卡号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);  //57卡售水量
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strtotal);//57卡售水总量
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	if(netisbreak==0)     //通讯不正常
	{
		XFS_Fail_Play("57预付费通讯失败，重启中");
		CPU_Reset();
	}
	else  //通讯正常
	{
				Interger_Clear(num,20);
				Char_Find(num,RecieveServerBuf, '%');                                          //获取百分号的位置
				String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //取头
				if(strcmp(Card_RecvInfo.head,"SSend5557P")==0)                                  //判断为57卡
				{
					String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
					String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
					String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //取订单号
					String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //取充值量
					String_Find(Card_RecvInfo.total, RecieveServerBuf, num[4]+1, num[5]);           //取总量
					String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[5]+1, num[6]);  //取本次充值量便于语音播报
					/*将服务器下传数据保存起来*/
					strcpy(Card_SendInfo.TSendCID,"TSend5557PResult%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
					strcat(Card_SendInfo.TSendCID,"%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
					strcat(Card_SendInfo.TSendCID,"%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
					
					Card57_PreReadCID(&SInfo57pre);                                               //重新读卡再次验证该卡是否与服务器返回的卡号一致
					
					Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo57pre.cardId, 10); //将5字节卡号拆分成10个半字节数组便于转换成字符
					HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//将10个半字节数组卡号转成字符数组
					
					if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
					{
						strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
						strcpy((char *)Card_SendInfo.Card_CharInfo.strtotal, Card_RecvInfo.total);
						
						StrToHex(SInfo57pre.watersalesamount, Card_SendInfo.Card_CharInfo.strbuyamount,2);//将充值量字符串转换为十六进制
						StrToHex(SInfo57pre.total, Card_SendInfo.Card_CharInfo.strtotal,2);//将充值量字符串转换为十六进制
						k = 0;
						while (k<3)
						{
							Card_SendInfo.Card_Flag = Card57_PreWrite(SInfo57pre); //把购水量写卡
							k++;
							if (Card_SendInfo.Card_Flag == 1)
							{
								strcat(Card_SendInfo.TSendCID,"%Success%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
								if (RecvState == 0)
								{
									XFS_Fail_Play("通信失败，请重刷");     //网络正常，重新刷卡
								}
								else if (RecvState == 1)
								{
									if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
									{
										String_Clear(voicesucc, 30);
										strcpy(voicesucc,"您已成功充值");
										strcat(voicesucc, Card_RecvInfo.pricemountstr);
										strcat(voicesucc,"吨");
										XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
									}
									else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
									{
										XFS_Fail_Play("交易未受理，请重刷");
									}
								}
								
								break;
							}
							else
							{						
								if (k == 3)
								{
									XFS_Fail_Play("充值失败"); //语音播报，屏幕显示
									strcat(Card_SendInfo.TSendCID,"%Fail%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
									break;
								}
							}
						}
					}
					else
					{
						strcat(Card_SendInfo.TSendCID,"%Fail%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
						XFS_Fail_Play("请重刷");     //网络正常，重新刷卡
					}
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Pcheck")==0)//判断为57查询卡
				{
					String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
					String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
					String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //取用户号
					String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //取用户地址
					if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
					else islongthan12 = 0;
					String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //取用水性质
					String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //取用户水表中水的总量
					String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //取用户当年已购水量
					UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
					UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
					UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
					UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
					UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
					UserInfoDisplay.islongthan12 = islongthan12;
					UserInfoDisplay.WaterInfo.Unit = 1;
					HDMIShowUserDetail(UserInfoDisplay);
					XFS_Only_Play("微信缴费后请将卡取出，重新插入完成写卡操作");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr0")==0)
				{
					XFS_Fail_Play("未知用户，请营业厅办理");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr1")==0)
				{
					XFS_Fail_Play("请微信支付后再刷卡");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr3")==0)
				{
					XFS_Fail_Play("无效卡，请营业厅办理");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr12")==0)
				{
					XFS_Fail_Play("交易关闭，请营业厅办理");
				}
				else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
				{
					XFS_Fail_Play("网络服务未开启");
				}
				else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
				{
					XFS_Fail_Play("网络无响应");
				}
				else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
				{
						char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
						AT24C02_Write(100,clear,5);	//清空缓存中的头
				}
				else
				{
					
				}
	}
}
/*******************************************************************************
* 函 数 名         : Card57_LDeal
* 函数功能		     : 57卡阶梯处理函数
* 输    入         : Card57l：57卡阶梯结构体
* 输    出         : 无
*******************************************************************************/
void Card57_LDeal(Card57_LadderInfo *Card57l)
{
	Card57_LadderInfo   SInfo57Ladder;     //第二次读取
	
  u8 price[20];
	u8 amount[20];

	u8 k;
	u8  RecvState = 0;
	islongthan12 = 0;                  //字符长度超过12
	netisbreak = 0;                     //判断当前网络线路是否通


	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Card57l->cardId, 10); //将5字节卡号拆分成10个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//将10个半字节数组卡号转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Card57l->watersalesamount, 8);//将2字节售水量拆分成4个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 8);//将4个半字节数组售水量转成字符数组
	Byte_Split(Card_SendInfo.Card_HexInfo.hextotal, Card57l->total, 6);//将2字节售水总量拆分成4个半字节数组便于转换成字符
	HexToChar(Card_SendInfo.Card_CharInfo.strtotal, Card_SendInfo.Card_HexInfo.hextotal, 6);//将4个半字节数组售水总量转成字符数组
	
	strcpy(Card_SendInfo.TSendCID,"TSend5557LCID%");  //57卡阶梯CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //57卡阶梯机器号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //57卡卡号
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);  //57卡售水量
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strtotal);//57卡售水总量
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	
	if(netisbreak==0)  //通讯失败，重启
	{
			//M6311_ReConnect(DeviceInfo.ServerInfo.ServerIP, DeviceInfo.ServerInfo.ServerPort, "网络重连中...");  //M6311连接服务器
		  XFS_Fail_Play("57阶梯通讯失败，重启中");
			CPU_Reset();
	}
	else  //通讯正常，重启
	{
			Interger_Clear(num, 20);
			Char_Find(num,RecieveServerBuf, '%');                                          //获取百分号的位置
			String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //取头
			if(strcmp(Card_RecvInfo.head,"SSend5557L")==0)                                  //判断为57卡
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
				String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //取订单号
				String_Find(Card_RecvInfo.oneprice, RecieveServerBuf, num[3]+1, num[4]);        //取一价
				String_Find(Card_RecvInfo.twoprice, RecieveServerBuf, num[4]+1, num[5]);        //取二价
				String_Find(Card_RecvInfo.threeprice, RecieveServerBuf, num[5]+1, num[6]);      //取三价
				String_Find(Card_RecvInfo.oneamount, RecieveServerBuf, num[6]+1, num[7]);       //取一量
				String_Find(Card_RecvInfo.twoamount, RecieveServerBuf, num[7]+1, num[8]);       //取二量
				String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[8]+1, num[9]);       //取充值量
				String_Find(Card_RecvInfo.total, RecieveServerBuf, num[9]+1, num[10]);          //取总量
				String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[10]+1, num[11]);  //取本次充值量便于语音播报
				/*将服务器下传数据保存起来*/
				strcpy(Card_SendInfo.TSendCID,"TSend5557LResult%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
				
				Card57_LadderReadCID(&SInfo57Ladder);       //读57阶梯卡
				//XFS_HDMI_Play("57卡第二次读取成功");
				Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo57Ladder.cardId, 10); //将5字节卡号拆分成10个半字节数组便于转换成字符
				HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//将10个半字节数组卡号转成字符数组
				if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
				{
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneprice, Card_RecvInfo.oneprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoprice, Card_RecvInfo.twoprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeprice, Card_RecvInfo.threeprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneamount, Card_RecvInfo.oneamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoamount, Card_RecvInfo.twoamount);
					
					strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtotal, Card_RecvInfo.total);
					
					strcpy((char *)price,(char *)Card_SendInfo.Card_CharInfo.stroneprice);
					strcat((char *)price,(char *)Card_SendInfo.Card_CharInfo.strtwoprice);
					strcat((char *)price,(char *)Card_SendInfo.Card_CharInfo.strthreeprice);
					
					strcpy((char *)amount,(char *)Card_SendInfo.Card_CharInfo.stroneamount);
					strcat((char *)amount,(char *)Card_SendInfo.Card_CharInfo.strtwoamount);
					
					StrToHex(SInfo57Ladder.watersalesamount,Card_SendInfo.Card_CharInfo.strbuyamount,4);//将充值量字符串转换为十六进制
					StrToHex(SInfo57Ladder.total,Card_SendInfo.Card_CharInfo.strtotal,3);//将充值总量字符串转换为十六进制
					StrToHex(SInfo57Ladder.ladderprice,price,6);
					StrToHex(SInfo57Ladder.ladderamount,amount,3);
					
					k=0;
					while (k<3)
					{
						Card_SendInfo.Card_Flag = Card57_LadderWriteCID(SInfo57Ladder); //把购水量写卡
						k++;
						if (Card_SendInfo.Card_Flag == 1)
						{
							strcat(Card_SendInfo.TSendCID,"%Success%$$");
							strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
							RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
							if (RecvState == 0)
							{
								XFS_Fail_Play("通信失败，请重刷");     //网络正常，重新刷卡
							}
							else if (RecvState == 1)
							{
								if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
								{
									String_Clear(voicesucc,30);
									strcpy(voicesucc,"您已成功充值");
									strcat(voicesucc, Card_RecvInfo.pricemountstr);
									strcat(voicesucc,"元");
									XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 0);
								}
								else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
								{
									XFS_Fail_Play("交易未受理，请重刷");
								}
							}
							break;
						}
						else
						{
							if(k==3)
							{
								XFS_Fail_Play("充值失败"); //语音播报，屏幕显示
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
								break;
							}
						}
					}	
				}
				else
				{
					strcat(Card_SendInfo.TSendCID,"%Fail%$$");
					strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
					M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
					XFS_Fail_Play("请重刷");								
				}
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lcheck")==0)//判断为57查询卡
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //取机器号
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //取卡号
				String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //取用户号
				String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //取用户地址
				if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
				else islongthan12 = 0;
				String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //取用水性质
				String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //取用户水表中水的总量
				String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //取用户当年已购水量
				UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
				UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
				UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
				UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
				UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
				UserInfoDisplay.islongthan12 = islongthan12;
				UserInfoDisplay.WaterInfo.Unit = 0;
				HDMIShowUserDetail(UserInfoDisplay);
				XFS_Only_Play("微信缴费后请将卡取出，重新插入完成写卡操作");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr0")==0)
			{
				XFS_Fail_Play("未知用户，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr1")==0)
			{
				XFS_Fail_Play("请微信支付后再刷卡");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr3")==0)
			{
				XFS_Fail_Play("无效卡，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr12")==0)
			{
				XFS_Fail_Play("交易关闭，请营业厅办理");
			}
			else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
			{
				XFS_Fail_Play("网络服务未开启");
			}
			else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
			{
				XFS_Fail_Play("网络无响应");
			}
			else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
			{
					char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
					AT24C02_Write(100,clear,5);	//清空缓存中的头
			}
			else
			{
				
			}
	}
	
}
/*******************************************************************************
* 函 数 名         : CacheOrder_update
* 函数功能		     : 缓存账单更新 
* 输    入         : 
* 输    出         : 无
*******************************************************************************/
void CacheOrder_update(void)	
{
	char chead[10];//缓存中的头
	char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//用于将缓存中的头清空
	char crecieve[100];
	
	
	u8  i;
	String_Clear(chead,10);
	String_Clear(UploadServerBuf,100);
	Interger_Clear(num,20);
	
	AT24C02_Read(100,UploadServerBuf,100);                   //读取未更新成功的订单更新指令	
  Char_Find(num,UploadServerBuf, '%');                     //获取百分号的位置	
	String_Find(chead, UploadServerBuf, 0, num[0]);          //获取头
	
	if((strcmp(chead,"TSend")==0) &&(strcmp(UploadServerBuf,"$$")==0))
	{
		M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //上传数据到服务器
		for (i=0; i<20; i++)  //等待返回数据
		{
			if (usart2_rcv_len > 0) 
			{
				delay_ms(341);
				if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"update"))
				{
					break;
				}
			}
			delay_ms(600);
		}
	  if (i < 20)
		{
			String_Clear(RecieveServerBuf,100);
			M6311_USART_GetRcvData(RecieveServerBuf, usart2_rcv_len); //接收服务器订单更新结果
			if (strcmp(crecieve, "updateorderstateOK2")==0)
			{
				AT24C02_Write(100,clear,5);	//清空缓存中的头
			}
			else
			{
				CPU_Reset();
			}
		}
	}
	else
	{
		AT24C02_Write(100,clear,5);	
	}
}	

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
