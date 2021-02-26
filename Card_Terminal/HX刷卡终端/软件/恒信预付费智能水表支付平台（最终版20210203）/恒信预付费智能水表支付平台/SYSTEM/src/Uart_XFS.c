/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_XF.h
 * ��Ŀ���� �������崮��4������ģ������         
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "Uart_XFS.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* �� �� ��         : USART_XFS_Init
* ��������		     : ����4��ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void UART_XFS_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef UART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��GPIOCʱ��,ʹ�ܸ���ʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    //USART_DeInit(UART_XFS);  //��λ����4
	
		GPIO_InitStructure.GPIO_Pin=UART_XFS_TX;// TXΪ���츴�����
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; 
		GPIO_Init(UART_XFS_Port,&GPIO_InitStructure);
	
		GPIO_InitStructure.GPIO_Pin=UART_XFS_RX;// RXΪ��������
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
* �� �� ��         : UART_XFS_SendB
* ��������		     : �ֽڷ��ͺ�������������0xff������
* ��    ��         : Byte:�ֽ�
* ��    ��         : ��
*******************************************************************************/
void UART_XFS_SendB(u8 Byte)		
{		 
		USART_SendData(UART_XFS,Byte);  //����һ���ֽ�
		while(USART_GetFlagStatus(UART_XFS,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
 
} 
/*******************************************************************************
* �� �� ��         : UART_XFS_SendS
* ��������		     : �ַ������ͺ���
* ��    ��         : String:�ַ���
* ��    ��         : ��
*******************************************************************************/
void UART_XFS_SendS(char *String, u16 len)	
{
	u16 i;
	for(i=0;i<len;i++)
	{
		USART_SendData(UART_XFS,String[i]);  //����һ���ֽ�
		while(USART_GetFlagStatus(UART_XFS,USART_FLAG_TXE)==RESET);
	}
}

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
