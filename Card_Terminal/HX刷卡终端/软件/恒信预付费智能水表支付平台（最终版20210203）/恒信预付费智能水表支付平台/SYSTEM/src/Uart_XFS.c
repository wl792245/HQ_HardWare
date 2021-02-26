/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Uart_XF.h
 * 项目描述 ：开发板串口4与语音模块连接         
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Uart_XFS.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* 函 数 名         : USART_XFS_Init
* 函数功能		     : 串口4初始化
* 输    入         : Bound：波特率， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void UART_XFS_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef UART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能GPIOC时钟,使能复用时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //USART_DeInit(UART_XFS);  //复位串口4
	
		GPIO_InitStructure.GPIO_Pin=UART_XFS_TX;// TX为推挽复用输出
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; 
		GPIO_Init(UART_XFS_Port,&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin=UART_XFS_RX;// RX为上拉输入
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
		GPIO_Init(UART_XFS_Port,&GPIO_InitStructure);
	
		UART_InitStructure.USART_BaudRate=Bound;
		UART_InitStructure.USART_WordLength=USART_WordLength_8b;
		UART_InitStructure.USART_StopBits=USART_StopBits_1;
		UART_InitStructure.USART_Parity=USART_Parity_No;
		UART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
		UART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
		USART_Init(UART_XFS,&UART_InitStructure);
		
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
		NVIC_Init(&NVIC_InitStructure);
		
		USART_Cmd(UART_XFS, ENABLE);	
}
/*******************************************************************************
* 函 数 名         : UART_XFS_SendB
* 函数功能		     : 字节发送函数，发送三次0xff结束符
* 输    入         : Byte:字节
* 输    出         : 无
*******************************************************************************/
void UART_XFS_SendB(u8 Byte)		
{		 
		USART_SendData(UART_XFS,Byte);  //发送一个字节
		while(USART_GetFlagStatus(UART_XFS,USART_FLAG_TXE)==RESET){};//等待发送结束
 
} 
/*******************************************************************************
* 函 数 名         : UART_XFS_SendS
* 函数功能		     : 字符串发送函数
* 输    入         : String:字符串
* 输    出         : 无
*******************************************************************************/
void UART_XFS_SendS(char *String, u16 len)	
{
	u16 i;
	for(i=0;i<len;i++)
	{
		USART_SendData(UART_XFS,String[i]);  //发送一个字节
		while(USART_GetFlagStatus(UART_XFS,USART_FLAG_TXE)==RESET);
	}
}

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
