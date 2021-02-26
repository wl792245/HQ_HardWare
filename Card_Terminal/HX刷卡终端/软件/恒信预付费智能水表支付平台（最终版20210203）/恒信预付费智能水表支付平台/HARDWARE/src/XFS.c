/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：XFS.C
 * 项目描述 ：语音模块设计      
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "XFS.h"
#include "HDMI_T050.h"
#include "systick.h"
#include "string.h"

/*******************************************************************************
* 函 数 名         : XFS_Delay
* 函数功能		     : 语言模块延时
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void XFS_Delay(u32 Time)
{  
	u32 i=0,j=0;
	for(i=Time;i>0;i--)
		for(j=2000;j>0;j--);
}
/*******************************************************************************
* 函 数 名         : XFS_POWER_Init
* 函数功能		     : 语言模块电源初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void XFS_POWER_Init(void)	  
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); /* 开启GPIO时钟 */
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin= XFS_POWER ;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(XFS_Port,&GPIO_InitStructure); /* 初始化GPIO */		
}
/*******************************************************************************
* 函 数 名         : XFS_Init
* 函数功能		     : 语言模块初始化
* 输    入         : Bound：波特率， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void XFS_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	UART_XFS_Init(Bound, m_irq, s_irq);
	XFS_POWER_Init();
}
/*******************************************************************************
* 函 数 名         : XFS_Play
* 函数功能		     : 语音模块播放串口送过来的数据
* 输    入         : CQdata:字符串
* 输    出         : 无
*******************************************************************************/
static void UART_XFS_Play(char *CQdata, int Delay_Time)
{
	char Data_Head[80] = {'\0'};
	u16 len=0; 
	XFS_POWER_H;
	XFS_Delay(1000);
	len =strlen(CQdata);	//需要发送文本的长度           
	Data_Head[0] = 0xFD ;	//构造帧头FD
	Data_Head[1] = 0x00 ;	//构造数据区长度的高字节
	Data_Head[2] = len+2;	//构造数据区长度的低字节
	Data_Head[3] = 0x01 ;	//构造命令字：合成播放命令		 		 
	Data_Head[4] = 0x01;	//文本编码格式：GBK 
	memcpy(&Data_Head[5], CQdata, len);
	UART_XFS_SendS(Data_Head,5+len);
	XFS_Delay(Delay_Time); //语音播放时间长点
	XFS_POWER_L;
	delay_ms(941);
}
/*******************************************************************************
* 函 数 名         : XFS_HDMI_Play
* 函数功能		     : 语音模块播放和液晶模块显示
* 输    入         : CQdata:字符串
* 输    出         : 无
*******************************************************************************/
void XFS_HDMI_Play(char *CQdata)
{
	HDMIShowInfo(CQdata);
	UART_XFS_Play(CQdata, 20000);
}
/*******************************************************************************
* 函 数 名         : XFS_Only_Play
* 函数功能		     : 仅进行语言播报
* 输    入         : CQdata:字符串
* 输    出         : 无
*******************************************************************************/
void XFS_Only_Play(char *CQdata)
{
	UART_XFS_Play(CQdata, 35000);
}
/*******************************************************************************
* 函 数 名         : XFS_Success_Play
* 函数功能		     : 充值成功播报
* 输    入         : CQdata:字符串 RechargeSucessValue:充值金额  Unit：0-元，1-吨
* 输    出         : 无
*******************************************************************************/
void XFS_Success_Play(char *CQdata,char *RechargeSucessValue,int Unit)
{
	HDMIShowRechargeSucess(RechargeSucessValue, Unit);
	UART_XFS_Play(CQdata, 20000);
}
/*******************************************************************************
* 函 数 名         : XFS_Fail_Play
* 函数功能		     : 充值失败播报
* 输    入         : CQdata:字符串
* 输    出         : 无
*******************************************************************************/
void XFS_Fail_Play(char *CQdata)
{
	HDMIShowInfo(CQdata);
	UART_XFS_Play(CQdata, 20000);
}

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
