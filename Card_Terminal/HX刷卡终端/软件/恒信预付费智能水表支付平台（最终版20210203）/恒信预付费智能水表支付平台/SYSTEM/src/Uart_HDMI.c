/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Uart_HDMI.c
 * 项目描述 ：开发板串口5与液晶屏幕连接         
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Uart_HDMI.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* 函 数 名         : UART_HDMI_Init
* 函数功能		     : 串口5初始化
* 输    入         : Bound：波特率（9600）， m_irq：中断优先级，s_irq：响应优先级
* 输    出         : 无
*******************************************************************************/
void UART_HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//使能PORTC，PORTD时钟
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //使能UART5
	  USART_DeInit(UART_HDMI);  //复位串口5
	
	  //UART5_TX   PC.12
		GPIO_InitStructure.GPIO_Pin = UART_HDMI_TX; //PC.12
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
		GPIO_Init(UART_HDMI_TX_Port, &GPIO_InitStructure); //初始化PC12
	  //UART5_RX	  PD.2
		GPIO_InitStructure.GPIO_Pin = UART_HDMI_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
		GPIO_Init(UART_HDMI_RX_Port, &GPIO_InitStructure);  //初始化PD2 

	  UART_InitStructure.USART_BaudRate = Bound;
		UART_InitStructure.USART_WordLength = USART_WordLength_8b;
		UART_InitStructure.USART_StopBits = USART_StopBits_1;
		UART_InitStructure.USART_Parity = USART_Parity_No;
		UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init(UART_HDMI, &UART_InitStructure);

		
		NVIC_InitStructure.NVIC_IRQChannel = UART_HDMI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= m_irq ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
		//USART_ITConfig(UART_HDMI, USART_IT_RXNE, ENABLE);//开启中断
		USART_Cmd(UART_HDMI, ENABLE); 
}
/*******************************************************************************
* 函 数 名         : UART_HDMI_SendS
* 函数功能		     : 字符串发送函数
* 输    入         : String:字符串
* 输    出         : 无
*******************************************************************************/
void UART_HDMI_SendS(char *String)	
{
	while(*String != '\0')
	{
		USART_SendData(UART_HDMI,*String);  //发送一个字节
		
		while(USART_GetFlagStatus(UART_HDMI,USART_FLAG_TXE)==RESET)
		{
			;
		};//等待发送结束
		String++;
	}
}
/*******************************************************************************
* 函 数 名         : UART_HDMI_SendB
* 函数功能		     : 字节发送函数，发送三次0xff结束符
* 输    入         : Byte:字节
* 输    出         : 无
*******************************************************************************/
void UART_HDMI_SendB(u8 Byte, u8 Tim)		
{		 
	u8 i;
	for(i=0;i<Tim;i++)
	{
		if(Byte!=0)
		{
			USART_SendData(UART_HDMI,Byte);  //发送一个字节
			while(USART_GetFlagStatus(UART_HDMI,USART_FLAG_TXE)==RESET){};//等待发送结束
		}
		else 
		{
			return ;
		}
	} 
} 
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
