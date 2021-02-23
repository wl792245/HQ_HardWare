/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：PC_Config.c
 * 项目描述 ：PC模块功能定义     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#include "PC_Config.h"
#include "USB_Config.h"
#include "systick.h"
#include "string.h"
#include "String_Config.h"
#include "HDMI_T050.h"
#include "PAM8406_Config.h"

volatile unsigned char PC_Connecting = 1;   //微电脑正在连接中
volatile unsigned char PC_RestartFlag = 0;   //微电脑重启标志位
char SendBuf[220];  //发送缓冲区
/*******************************************************************************
* 函 数 名         : PCPOWER_Init
* 函数功能		     : PC电源引脚初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void PCPOWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//设置微控制器系统初始化嵌入式闪存接口、PLL并更新系统coclock变量。	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //启用或禁用高速apb（apb2）外围时钟。

	GPIO_InitStructure.GPIO_Pin=PC_POWER_PIN ;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽输出  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;	  //最高输出率50MHz
	GPIO_Init(PC_POWER_PORT,&GPIO_InitStructure); //根据gpio_init结构中的指定参数初始化gpiox外围。
}
/*******************************************************************************
* 函 数 名         : PC_Init
* 函数功能		     : PC模块初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	PCPOWER_Init();
	USART_PC_Init(Bound, m_irq, s_irq);
	USB_CONFIG_Init(m_irq,s_irq);
}
/*******************************************************************************
* 函 数 名         : PC_Close
* 函数功能		     : 微电脑关闭
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_Close(void)
{
	PC_POWER_L;
	delay_ms(1000);
}
/*******************************************************************************
* 函 数 名         : PC_Open
* 函数功能		     : 微电脑打开
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_Open(void)
{
	PC_POWER_H;
	delay_ms(1000);
}
/*******************************************************************************
* 函 数 名         : USART_PC_SendData
* 函数功能		     : 通过串口向电脑发送数据
* 输    入         : buf:字符串；len:长度
* 输    出         : 无
*******************************************************************************/
void USART_PC_SendData(char * buf,uint32_t len)
{
	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len=0;
	USART_PC_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* 函 数 名         : PC_ReadCard
* 函数功能		     : 单片机向电脑发送读卡命令用来读取4442和4428对的卡信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_ReadCard(void)
{
	PC_SendData("ReadCard%$$");
}
/*******************************************************************************
* 函 数 名         : PC_ReadData
* 函数功能		     : 通过串口或者USB向PC读取数据
* 输    入         : pdest:接收字符串  times：次数 len:接收长度
* 输    出         : 1:成功接收到数据 0：接收数据失败
*******************************************************************************/
u8 PC_ReadData(char * pdest,  int times,int len)
{
		u8 Sendtimes = 0;
	  u8 PC_ReturnValue = 0; 
	  if(Sendtimes < times) //没返回就连续发送3次
		{
			PC_ReadCard();         //上发读卡指令到电脑
			Sendtimes++;  
			PC_ReturnValue = PC_ReceiveData(pdest, len);
			if (1 == PC_ReturnValue)
			{
				return 1;
			}
		}
		return 0;
}
/*******************************************************************************
* 函 数 名         : PC_WriteCard
* 函数功能		     : 单片机向电脑发送写卡的命令
* 输    入         : psrc:发送信息，CardType:卡类型，0-42卡，1-28卡，2-57卡
* 输    出         : 无
*******************************************************************************/
void PC_WriteCard(const char *psrc, CardType_Info CardTypeInfo)
{
	String_Clear(SendBuf, strlen(SendBuf));
	switch(CardTypeInfo)
	{
		case HX4442:
			strcpy(SendBuf,"Write4442Card%");
			break;
		case HX4428:
			strcpy(SendBuf,"Write4428Card%");
			break;
		case XT5557:
			strcpy(SendBuf,"Writee57Card%");
			break;
		case SC5557:
			strcpy(SendBuf,"WriteSC5557Card%");
			break;
	}
	strcat(SendBuf,psrc);		//两个字符串合并
	strcat(SendBuf,"%$$");
	PC_SendData(SendBuf);

}
/*******************************************************************************
* 函 数 名         : PC_WirteData
* 函数功能		     : 通过串口或者USB口对PC发送写卡数据指令
* 输    入         : pdest:接收字符串；psrc:写入字符串  times：次数 len:接收长度 CardTypeInfo:卡类型
* 输    出         : 1:成功接收到数据 0：接收数据失败
*******************************************************************************/
u8 PC_WirteData(char * pdest, const char *psrc, int times,int len, CardType_Info CardTypeInfo)
{
	u8 Sendtimes = 0;
	u8 PC_ReturnValue = 0; 
	if (Sendtimes < times)
	{
		//向电脑发送写卡命令
		PC_WriteCard(psrc, CardTypeInfo);
		Sendtimes++; 
		PC_ReturnValue = PC_ReceiveData(pdest, len);
		if (PC_ReturnValue == 1)
		{
			return 1;
		}
	}
	else
	{
		
	}
	return 0;
}
/*******************************************************************************
* 函 数 名         : PC_ShutDown
* 函数功能		     : 单片机向电脑发送关机命令
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_ShutDown(void)
{
	PC_SendData("ShutDown%$$");
}
/*******************************************************************************
* 函 数 名         : PC_SendData
* 函数功能		     : 单片机向电脑发送指令
* 输    入         : data：握手指令
* 输    出         : 无
*******************************************************************************/
void PC_SendData(char * data)
{
	USB_PC_SendData(data);
  USART_PC_SendData(data,strlen(data));
}
/*******************************************************************************
* 函 数 名         : PC_ReceiveData
* 函数功能		     : 单片机接收电脑传回的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 PC_ReceiveData(char * pdest, int len)
{
	u8 i;
	for (i=0; i<20; i++)  //等待返回数据
	{
		if (usart2_rcv_len > 0) 
		{
			delay_ms(341);
			if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"%$$"))
			{
				String_Clear(pdest, len);
				PC_Usart_GetRcvData(pdest, usart2_rcv_len);
				break;
			}				
		}
		else if (USB_USART_RX_STA > 0) 
		{
			delay_ms(341);
			if (NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"%$$"))
			{
				String_Clear(pdest, len);
				PC_USB_GetRcvData(pdest, USB_USART_RX_STA);
				break;
			}				
		}
		delay_ms(600);
	}
	if (i < 20)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
/*******************************************************************************
* 函 数 名         : PC_Usart_GetRcvData
* 函数功能		     : 单片机通过串口接收到电脑的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_Usart_GetRcvData(char * buf, int rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = usart2_rcv_buf[i];
	}
	buf[i] = '\0';
	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len = 0;
}
/*******************************************************************************
* 函 数 名         : PC_USB_GetRcvData
* 函数功能		     : 单片机通过串口接收到电脑的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_USB_GetRcvData(char * buf, int rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = USB_USART_RX_BUF[i];
	}
	buf[i] = '\0';
	String_Clear((char *)USB_USART_RX_BUF,strlen((const char *)USB_USART_RX_BUF));
	USB_USART_RX_STA = 0;
}
/*******************************************************************************
* 函 数 名         : PC_StartPam
* 函数功能		     : 单片机向电脑发送语音播报命令
* 输    入         : txt：语音播报文本
* 输    出         : 无
*******************************************************************************/
void PC_StartPam(char *txt)
{
	int i;
	String_Clear(SendBuf, strlen(SendBuf));
	PAM8406_Open();
	strcpy(SendBuf,"StartPam%");
	strcat(SendBuf,txt);		//两个字符串合并
	strcat(SendBuf,"%$$");
	PC_SendData(SendBuf);
	for(i=0;i<7;i++)
	{
		delay_ms(1431);	
	}
	PAM8406_Close();
}
/*******************************************************************************
* 函 数 名         : PC_Start
* 函数功能		     : 使用USB或者串口模式与电脑通讯判断是否启动
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PC_Start(void)
{
	char i;
	u8 pcisbreak, Sendtimes;
	
	PC_Connecting = 1;
	PC_RestartFlag = 0;
	Sendtimes = 0;
	pcisbreak =0;
	
	HDMIShowInfo("微电脑连接中，请稍等…");
	//等电脑启动完，可以换成接收启动成功之后发送字符来判断
	for(i=0;i<31;i++)
	{
			delay_ms(941);
	}
	while (Sendtimes < 3)
	{
		delay_ms(100);
		PC_SendData("Hello%$$");
		Sendtimes++;
		for (i=0; i<20; i++)  //等待返回数据
		{
			if ((usart2_rcv_len > 0) || (USB_USART_RX_STA > 0)) 
			{
				delay_ms(341);
				if ((NULL != strstr((const char *)usart2_rcv_buf, (const char *)"Too"))||(NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"Too")))
				{
					delay_ms(100);
					break;
				}				
			}
			delay_ms(600);
		}
		if (i<20)  
		{
				pcisbreak = 1;
				break;
		}
	}
	if(pcisbreak == 1)
	{
		PC_Connecting = 0;
		PC_RestartFlag = 0;
		HDMIShowInfo("设备启动成功");
	}
	else
	{
		PC_Connecting = 1;
		HDMIShowInfo("系统重启中…");
		PC_ShutDown();
		for(i=0;i<31;i++)
		{
			delay_ms(941);
		}
		PC_RestartFlag = 1;  //电脑重启标志位
	}
}

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
