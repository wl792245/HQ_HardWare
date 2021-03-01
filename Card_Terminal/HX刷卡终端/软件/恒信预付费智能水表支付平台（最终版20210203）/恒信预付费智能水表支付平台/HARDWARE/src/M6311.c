/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：M6311.c
 * 项目描述 ：M6311模块功能定义     
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#include "M6311.h"
#include "HDMI_T050.h"
#include "stm32f10x.h"
#include "systick.h"
#include "string.h"
#include "String_Get.h"
#include "utils.h"
#include "XFS.h"
char M6311ConfigMsg[80];  //IP和端口字符组合
char M6311_RecieveServerBuf[50];//接收服务器返回的结果 
unsigned char M6311_Signal = 31;      //开始默认信号强度
unsigned char M6311_Connecting = 1;   //正在连接中
volatile unsigned char M6311_RestartFlag = 0;  //M6311重启标志位

/*******************************************************************************
* 函 数 名         : M6311POWER_Init
* 函数功能		     : M6311电源引脚初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void M6311POWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//设置微控制器系统初始化嵌入式闪存接口、PLL并更新系统coclock变量。	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);  //启用或禁用高速apb（apb2）外围时钟。
	
	GPIO_InitStructure.GPIO_Pin=M6311_POWER_PIN ;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽输出  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //最高输出率50MHz
	GPIO_Init(M6311_POWER_PORT,&GPIO_InitStructure); //根据gpio_init结构中的指定参数初始化gpiox外围。
}
/*******************************************************************************
* 函 数 名         : M6311_Init
* 函数功能		     : 2G模块初始化
* 输    入         : Bound：波特率， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void M6311_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	USART_M6311_Init(Bound, m_irq, s_irq);
	M6311POWER_Init();
}
/*******************************************************************************
* 函 数 名         : M6311_ReConnect
* 函数功能		     : M6311重新连接
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static int M6311_RConnect(void)
{
	char i,j = 0;
	M6311_Connecting = 1;
	M6311_POWER_L;		//开GPRS模块电源
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
	M6311_POWER_H;
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
 	//添加网络连接中提示
	for(i=0; i < 18;i++)    //每9次重新启动网络
	{	
		for(j=0;j<30;j++)			//等待接收数据结束
		{
			if(usart2_rcv_len>0)
			{
				delay_ms(341);
				break;
			}
			delay_ms(341);
		}
		if(j < 30)
		{
			if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"CTZV"))
			{
				return 1;
			}
			usart2_rcv_len = 0;
		}

	}
	return 0;
}
/*******************************************************************************
* 函 数 名         : M6311_Connect
* 函数功能		     : 2G模块连接
* 输    入         : IP:IP地址；Port:端口号
* 输    出         : 无
*******************************************************************************/
void M6311_Connect(const pDeviceInit pServerInfo, char *ShowInfo)
{
  int isStartNet = 1;      //启动是否成功标志位
  int isConnect = 0;
	HDMIShowInfo(ShowInfo);
	while (isStartNet)
	{
		isConnect = M6311_RConnect();
		if(isConnect)
		{
			//上电初始化
			if(USART_M6311_SendCmd("AT\r\n","OK",842)==0) //测试AT,该命令用来检验是否能和 DCE 模块正常通讯
			{
				continue; 			
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+IPR=115200;&W \r\n","OK",842)==0) //用来设置或读取 DCE 的波特率。在设置了波特率后，对应的工具如超级
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGREG?\r\n","+CGREG: 0,1",842)==0)  //  设置 MT 显示网络注册状态和位置信息
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK", 862)==0) /*MT 发送 PDP 上下文激活消息的时候使用这个命令配置
														PDP 上下文参数。系统重启动后，该命令所做的设置将不被保存*/
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGACT=1,1 \r\n","OK", 941)==0) //0：PDP 上下文去活,1：PDP 上下文激活
			{
				continue; 
			}
			delay_ms(941);
			//建立TCP连接
			if(USART_M6311_SendCmd("AT+CMMUX=0\r\n","OK",862)==0) //控制是否启用多路连接
			{
				continue; 
			}
			delay_ms(941); 
			if(USART_M6311_SendCmd("AT+CMMODE=1\r\n","OK",862)==0) /*	1是透传模式（在该模式下，建立连接成功后， 
														UART 将会进入数据模式。接收到的数据将直接输出至串口，
														输入数据会发送至服务器，可通过“+++”退出数据模式，
														退出后请使用 AT+IPCLOSE 释放资源）*/
			{
				continue; 
			}
			delay_ms(941);
			
			if(USART_M6311_SendCmd("AT+CMTCFG=1,1024\r\n","OK",862)==0) //配置 TCPIP 透传模式
			{
				continue; 
			}
			delay_ms(941); 

			strcpy(M6311ConfigMsg,"AT+IPSTART=\"TCP\",\"");//建立 TCP 连接或注册 UDP 端口号
			strcat(M6311ConfigMsg,pServerInfo->ServerInfo.ServerIP);
			strcat(M6311ConfigMsg,"\",");
			strcat(M6311ConfigMsg,pServerInfo->ServerInfo.ServerPort);
			strcat(M6311ConfigMsg,"\r\n");
			if(USART_M6311_SendCmd(M6311ConfigMsg,"CONNECT",941)==0)//30000->3000，20181022，3秒足够
			{
				continue; //网络故障，系统复位重启
			}
			M6311_SignalQuery(pServerInfo->TerminalInfo.SignalStrength);
			if (0x63 == M6311_RestartFlag)
			{
				continue;
			}
			Restart_Count = ReStart_Read(DeviceInfo.TerminalInfo.ReStart); //获取重启次数
			Restart_Count +=1;
			ResStart_Write(DeviceInfo.TerminalInfo.ReStart, Restart_Count);  //写入重启次数
			isStartNet = 0;
			HDMISendConncetOK();
			delay_ms(941);
		}
	}
}
/*******************************************************************************
* 函 数 名         : M6311_SendData
* 函数功能		     : 通过M6311向服务器发送数据
* 输    入         : buf:字符串；len:长度
* 输    出         : 无
*******************************************************************************/
void M6311_SendData(char * buf,uint32_t len)
{

	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len=0;
	USART_M6311_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* 函 数 名         : M6311_UploadData
* 函数功能		     : 通过M6311向服务器上传数据
* 输    入         : pdest:接收字符串；psrc:上传字符串  len:接收长度
* 输    出         : 1:成功接收到数据 0：接收数据失败
*******************************************************************************/
u8 M6311_UploadData(char * pdest,char *psrc, char *txt, int times,int len)
{
		u8 Sendtimes = 0;
	  u8 i;
	  while(Sendtimes < times) //没返回就连续发送3次
		{
			M6311_SendData(psrc, strlen(psrc)); //上传数据到服务器
			Sendtimes++;  
			for (i=0; i<20; i++)  //等待返回数据
			{
				if (usart2_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)txt))
					{
						String_Clear(pdest, len);
						M6311_USART_GetRcvData(pdest, usart2_rcv_len);
						return 1;
					}
				}
				delay_ms(600);
			}
		}
		return 0;
}

/*******************************************************************************
* 函 数 名         : M6311_USART_GetRcvData
* 函数功能		     : 服务器通过M6311向单片机串口发送信息
* 输    入         : buf：单片机接收数据数组， len：数据长度
* 输    出         : 无
*******************************************************************************/
void M6311_USART_GetRcvData(char *buf, uint32_t rcv_len)
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
* 函 数 名         : M6311_SignalQuery
* 函数功能		     : 查询信号强度 
* 输    入         : Signal：信号强度值
* 输    出         : 无
*******************************************************************************/
void M6311_SignalQuery(char *Signal)
{
	u8 i,j,k;
	
	M6311_RestartFlag = 0;
	k = 0;
	while(k<3)
	{
		k++;
		if(USART_M6311_SendCmd("+++","OK",842)==1)//查询信号强度
		{
			break;
		}
	}
	if (k >= 3)
	{
		M6311_RestartFlag = 0x63;
	}
	else
	{
		k = 0;
		while (k<3)
		{
			k++;
			if(USART_M6311_SendCmd("AT+CSQ\r\n","CSQ",842)==1)//查询信号强度
			{
				break;
			}
		}
		if (k >= 3)
		{
			M6311_RestartFlag = 0x63;  //
		}
		else
		{
			String_Clear(M6311_RecieveServerBuf, 50);
			M6311_USART_GetRcvData(M6311_RecieveServerBuf, usart2_rcv_len);
			i=0;
			while (M6311_RecieveServerBuf[i]!=':') 
			{
				i++; 
			}
			i++;
			while (M6311_RecieveServerBuf[i]!=' ')
			{
				i++; 
			}
			j=0;
			i++;
			while (M6311_RecieveServerBuf[i]!=',') 
			{						
				Signal[j]=M6311_RecieveServerBuf[i];//取信号强度
				i++;j++;
			}
			Signal[i]='\0';
			if(USART_M6311_SendCmd("ATO\r\n","OK",842)==1)// 回到数据连接模式
			{
				StrToDec(&M6311_Signal,Signal);	
			}
			else
			{
				M6311_RestartFlag = 0x63;  //
			}
		}
	}
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
