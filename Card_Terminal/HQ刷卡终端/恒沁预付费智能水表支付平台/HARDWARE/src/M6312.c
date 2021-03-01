/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：M6312.c
 * 项目描述 ：M6312模块功能定义     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：M6312(RX)--PA9 M6312(TX)--PA10 M6312(POWER)--PA15
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#include "M6312.h"
#include "HDMI_T050.h"
#include "stm32f10x.h"
#include "systick.h"
#include "string.h"
#include "PC_Config.h"
#include "String_Config.h"
#include "utils.h"
//#include "XFS.h"
char M6312ConfigMsg[80];  //IP和端口字符组合
char M6312_RecieveServerBuf[50];//接收服务器返回的结果 
 unsigned char M6312_Signal = 10;      //开始默认信号强度
volatile unsigned char M6312_RestartFlag = 0;      //M6312重启标志位
 unsigned char M6312_Connecting = 1;   //正在连接中
/*******************************************************************************
* 函 数 名         : M6312POWER_Init
* 函数功能		     : M6312电源引脚初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void M6312POWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//设置微控制器系统初始化嵌入式闪存接口、PLL并更新系统coclock变量。	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);  //启用或禁用高速apb（apb2）外围时钟。
	//如果你用PB3，PB4，PA15做普通IO，PA13&14用于SWD调试，
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin=M6312_POWER_PIN;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽输出  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //最高输出率50MHz
	GPIO_Init(M6312_POWER_PORT,&GPIO_InitStructure); //根据gpio_init结构中的指定参数初始化gpiox外围。
  M6312_POWER_L;		//开GPRS模块电源
}
/*******************************************************************************
* 函 数 名         : M6312_Init
* 函数功能		     : 2G模块初始化
* 输    入         : Bound：波特率， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void M6312_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	M6312POWER_Init();
	USART_M6312_Init(Bound, m_irq, s_irq);
}
/*******************************************************************************
* 函 数 名         : M6312_ReConnect
* 函数功能		     : M6312重新连接
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static int M6312_RConnect(void)
{
	char i,j = 0;
	M6312_Connecting = 1;
	M6312_POWER_L;		//开GPRS模块电源
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
	M6312_POWER_H;
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
 	//添加网络连接中提示
	for(i=0; i < 18;i++)    //每9次重新启动网络
	{	
		for(j=0;j<30;j++)			//等待接收数据结束
		{
			if(usart1_rcv_len>0)
			{
				delay_ms(341);
				break;
			}
			delay_ms(341);
		}
		if(j < 30)
		{
			if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"CTZV"))
			{
				return 1;
			}
			usart1_rcv_len = 0;
		}

	}
	return 0;
}
/*******************************************************************************
* 函 数 名         : M6312_Connect
* 函数功能		     : 2G模块连接
* 输    入         : IP:IP地址；Port:端口号
* 输    出         : 无
*******************************************************************************/
void M6312_Connect(pDeviceInit pServerInfo,char *ShowInfo)
{
  int isStartNet = 1;      //启动是否成功标志位
  int isConnect = 0;

  M6312_Signal = 0;
	HDMIShowInfo(ShowInfo);

	while (isStartNet)
	{
		isConnect = M6312_RConnect();
		delay_ms(941);
		if(isConnect)
		{
			//上电初始化
			if(USART_M6312_SendCmd("AT\r\n","OK",842)==0) //测试AT,该命令用来检验是否能和 DCE 模块正常通讯
			{
				continue; 			
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CPIN?\r\n","READY",842)==0) //返回 READY，表示 SIM 卡正常识别运行K
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGREG?\r\n","+CGREG: 0,1",842)==0) //确认网络搜索成功,1或5表示注册成功
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK", 862)==0) //设置 GPRS 的 APN（请配置为 SIM 卡签约对应的 APN）
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGACT=1,1\r\n","OK", 941)==0) //激活 PDP（激活 GPRS 网络）
			{
				continue;
			}
			delay_ms(941);
			//建立TCP连接
			if(USART_M6312_SendCmd("AT+CMMUX=0\r\n","OK",862)==0) //控制是否启用多路连接
			{
				continue;
			}
			delay_ms(941); 
			if(USART_M6312_SendCmd("AT+CMHEAD=1\r\n","OK",862)==0) /*	1是透传模式（在该模式下，建立连接成功后， 
														UART 将会进入数据模式。接收到的数据将直接输出至串口，
														输入数据会发送至服务器，可通过“+++”退出数据模式，
														退出后请使用 AT+IPCLOSE 释放资源）*/
			{
				continue;
			}
			delay_ms(941);
			
			if(USART_M6312_SendCmd("AT+CMSHOWRA=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941); 
			
			if(USART_M6312_SendCmd("AT+CMSHOWLA=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941); 
			
			if(USART_M6312_SendCmd("AT+CMSHOWPT=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941);
			//配置透传
			if(USART_M6312_SendCmd("AT+CMMODE=1\r\n","OK",862)==0) //配置 TCPIP 透传模式
			{
				continue;
			}
			delay_ms(941);
			strcpy(M6312ConfigMsg,"AT+IPSTART=\"TCP\",\"");//建立 TCP 连接或注册 UDP 端口号
			strcat(M6312ConfigMsg,pServerInfo->ServerInfo.ServerIP);
			strcat(M6312ConfigMsg,"\",");
			strcat(M6312ConfigMsg,pServerInfo->ServerInfo.ServerPort);
			strcat(M6312ConfigMsg,"\r\n");
			if(USART_M6312_SendCmd(M6312ConfigMsg,"CONNECT",941)==0)//30000->3000，20181022，3秒足够
			{
				continue;//网络故障，系统复位重启
			}
			//连接成功
			//HDMIShowInfo("连接成功");
			M6312_SignalQuery(pServerInfo->TerminalInfo.SignalStrength);  //信号强度检测
			if (0x63 == M6312_RestartFlag)
			{
				continue;
			}
			Restart_Count = ReStart_Read(DeviceInfo.TerminalInfo.ReStart); //获取重启次数
			Restart_Count +=1;
			ResStart_Write(DeviceInfo.TerminalInfo.ReStart, Restart_Count);  //写入重启次数
			isStartNet = 0;
			delay_ms(941);
		}
	}
}
/*******************************************************************************
* 函 数 名         : M6312_SendData
* 函数功能		     : 通过M6312向服务器发送数据
* 输    入         : buf:字符串；len:长度
* 输    出         : 无
*******************************************************************************/
void M6312_SendData(char * buf,uint32_t len)
{

	String_Clear((char *)usart1_rcv_buf,strlen((const char *)usart1_rcv_buf));
	usart1_rcv_len=0;
	USART_M6312_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* 函 数 名         : M6312_UploadData
* 函数功能		     : 通过M6312向服务器上传数据
* 输    入         : pdest:接收字符串；psrc:上传字符串  txt：字符，times：次数 len:接收长度
* 输    出         : 1:成功接收到数据 0：接收数据失败
*******************************************************************************/
u8 M6312_UploadData(char * pdest,char *psrc, char *txt, int times,int len)
{
		u8 Sendtimes = 0;
	  u8 netisbreak = 0;
	  u8 i;
	  while(Sendtimes < times) //没返回就连续发送3次
		{
			M6312_SendData(psrc, strlen(psrc)); //上传数据到服务器
			Sendtimes++;  
			for (i=0; i<10; i++)  //等待返回数据
			{
				if (usart1_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)txt))
					{
						break;
					}
				}
				delay_ms(500);
			}
			if (i < 10)
			{
				netisbreak = 1;
				break;
			}
			
		}
		if (netisbreak == 1)
		{
				String_Clear(pdest, len);
				M6312_USART_GetRcvData(pdest, usart1_rcv_len);
				return 1;
		}
		else
		{
			//通讯失败
			PC_SendData("CardOff%$$");
			HDMIShowInfo("网络无响应");
			delay_ms(10);
			PC_StartPam("网络无响应");
			delay_ms(100);
			return 0;
		}
		
}
/*******************************************************************************
* 函 数 名         : M6312_USART_GetRcvData
* 函数功能		     : 服务器通过M6312向单片机串口发送信息
* 输    入         : buf：单片机接收数据数组， len：数据长度
* 输    出         : 无
*******************************************************************************/
void M6312_USART_GetRcvData(char *buf, uint32_t rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = usart1_rcv_buf[i];
	}
	buf[i] = '\0';
	String_Clear((char *)usart1_rcv_buf,strlen((const char *)usart1_rcv_buf));
	usart1_rcv_len = 0;
}
/*******************************************************************************
* 函 数 名         : M6312_SignalQuery
* 函数功能		     : 查询信号强度
* 输    入         : Signal:信号强度值
* 输    出         : 无
*******************************************************************************/
void M6312_SignalQuery(char *Signal)
{
	u8 i,j,k;


	k = 0;
	M6312_RestartFlag = 0;
	while(k<3)
	{
		k++;
		USART_M6312_Write("+++", strlen("+++"));
		delay_ms(999);
		if(USART_M6312_SendCmd("AT+CSQ\r\n","CSQ",1000)==1)//查询信号强度
		{
			break;
		}
	}
	if(k >= 3)
	{
		M6312_RestartFlag = 0x63;
	}
	else
	{
		String_Clear(M6312_RecieveServerBuf, 50);
		M6312_USART_GetRcvData(M6312_RecieveServerBuf, usart1_rcv_len);
		i=0;
		while (M6312_RecieveServerBuf[i]!=':') 
		{
			i++; 
		}
		i++;
		while (M6312_RecieveServerBuf[i]!=' ')
		{
			i++; 
		}
		j=0;
		i++;
		while (M6312_RecieveServerBuf[i]!=',') 
		{						
			Signal[j]=M6312_RecieveServerBuf[i];//取信号强度
			i++;j++;
		}
		Signal[i]='\0';
		//时间查询
		if(1 == M6312_TimeQuery(&DeviceInfo.DataInfo))
		{
			M6312_RestartFlag = 0x63;
		}
		if(USART_M6312_SendCmd("ATO\r\n","OK",1000)==1)// 回到数据连接模式
		{
			StrToDec(&M6312_Signal,Signal);
		}
		else
		{
			M6312_RestartFlag = 0x63;
		}
	}
}
/*******************************************************************************
* 函 数 名         : M6312_TimeQuery
* 函数功能		     : 时间检测
* 输    入         : TimeInit：时间指针结构体
* 输    出         : 1：读取失败， 0：读取成功
*******************************************************************************/
unsigned char M6312_TimeQuery(PDataInit TimeInit)
{
	u8 i,j,k;
	k = 0;
	M6312_RestartFlag = 0;
	while(k<3)
	{
		k++;
		if(USART_M6312_SendCmd("AT+CCLK?\r\n","OK",1000)==1)//时间检测
		{
			break;
		}
	}
	if (k >= 3)
	{
		return 1;
	}
	else
	{
		String_Clear(M6312_RecieveServerBuf, 50);
		M6312_USART_GetRcvData(M6312_RecieveServerBuf, usart1_rcv_len);
		i=0;
		while (M6312_RecieveServerBuf[i]!='"')
		{
			i++; 
		}
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]=='/') break;
			TimeInit->Year[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Year[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]=='/') break;
			TimeInit->Mounth[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Mounth[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==',') break;
			TimeInit->Day[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Day[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==':') break;
			TimeInit->Hour[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Hour[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==':') break;
			TimeInit->Minute[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Minute[j]='\0';
		return 0;
	}
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
