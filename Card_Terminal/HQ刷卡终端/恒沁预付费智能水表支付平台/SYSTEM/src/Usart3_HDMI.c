/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Uart_HDMI.c
 * 项目描述 ：开发板串口3与液晶屏幕连接         
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-25
**********************************************************************************/
#include "Usart3_HDMI.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* 函 数 名         : USART_HDMI_Init
* 函数功能		     : 串口3初始化
* 输    入         : Bound：波特率（9600）
* 输    出         : 无
*******************************************************************************/
void USART_HDMI_Init(u32 Bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef UART_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  USART_DeInit(USART_HDMI);  //复位串口3
	
	  //UART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = USART_HDMI_TX; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(USART_HDMI_Port, &GPIO_InitStructure); //初始化
	  //UART3_RX	  PB11
		GPIO_InitStructure.GPIO_Pin = USART_HDMI_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(USART_HDMI_Port, &GPIO_InitStructure);  //初始化

	  UART_InitStructure.USART_BaudRate = Bound;
		UART_InitStructure.USART_WordLength = USART_WordLength_8b;
		UART_InitStructure.USART_StopBits = USART_StopBits_1;
		UART_InitStructure.USART_Parity = USART_Parity_No;
		UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init(USART_HDMI, &UART_InitStructure);
		USART_Cmd(USART_HDMI, ENABLE); 
}
/*******************************************************************************
* 函 数 名         : USART_HDMI_SendS
* 函数功能		     : 字符串发送函数
* 输    入         : String:字符串
* 输    出         : 无
*******************************************************************************/
void USART_HDMI_SendS(char *String)	
{
	while(*String != '\0')
	{
		USART_SendData(USART_HDMI,*String);  //发送一个字节
		
		while(USART_GetFlagStatus(USART_HDMI,USART_FLAG_TXE)==RESET)
		{
			;
		};//等待发送结束
		String++;
	}
}
/*******************************************************************************
* 函 数 名         : USART_HDMI_SendB
* 函数功能		     : 字节发送函数，发送三次0xff结束符
* 输    入         : Byte:字节
* 输    出         : 无
*******************************************************************************/
void USART_HDMI_SendB(u8 Byte, u8 Tim)		
{		 
	u8 i;
	for(i=0;i<Tim;i++)
	{
		if(Byte!=0)
		{
			USART_SendData(USART_HDMI,Byte);  //发送一个字节
			while(USART_GetFlagStatus(USART_HDMI,USART_FLAG_TXE)==RESET){};//等待发送结束
		}
		else 
		{
			return ;
		}
	} 
} 
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
