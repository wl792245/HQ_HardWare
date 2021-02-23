/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_HDMI.c
 * ��Ŀ���� �������崮��3��Һ����Ļ����         
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-25
**********************************************************************************/
#include "Usart3_HDMI.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* �� �� ��         : USART_HDMI_Init
* ��������		     : ����3��ʼ��
* ��    ��         : Bound�������ʣ�9600��
* ��    ��         : ��
*******************************************************************************/
void USART_HDMI_Init(u32 Bound)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef UART_InitStructure;
	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  USART_DeInit(USART_HDMI);  //��λ����3
	
	  //UART3_TX   PB.10
		GPIO_InitStructure.GPIO_Pin = USART_HDMI_TX; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(USART_HDMI_Port, &GPIO_InitStructure); //��ʼ��
	  //UART3_RX	  PB11
		GPIO_InitStructure.GPIO_Pin = USART_HDMI_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(USART_HDMI_Port, &GPIO_InitStructure);  //��ʼ��

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
* �� �� ��         : USART_HDMI_SendS
* ��������		     : �ַ������ͺ���
* ��    ��         : String:�ַ���
* ��    ��         : ��
*******************************************************************************/
void USART_HDMI_SendS(char *String)	
{
	while(*String != '\0')
	{
		USART_SendData(USART_HDMI,*String);  //����һ���ֽ�
		
		while(USART_GetFlagStatus(USART_HDMI,USART_FLAG_TXE)==RESET)
		{
			;
		};//�ȴ����ͽ���
		String++;
	}
}
/*******************************************************************************
* �� �� ��         : USART_HDMI_SendB
* ��������		     : �ֽڷ��ͺ�������������0xff������
* ��    ��         : Byte:�ֽ�
* ��    ��         : ��
*******************************************************************************/
void USART_HDMI_SendB(u8 Byte, u8 Tim)		
{		 
	u8 i;
	for(i=0;i<Tim;i++)
	{
		if(Byte!=0)
		{
			USART_SendData(USART_HDMI,Byte);  //����һ���ֽ�
			while(USART_GetFlagStatus(USART_HDMI,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
		}
		else 
		{
			return ;
		}
	} 
} 
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
