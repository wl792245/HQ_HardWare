/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_HDMI.c
 * ��Ŀ���� �������崮��5��Һ����Ļ����         
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "Uart_HDMI.h"
#include "stm32f10x_usart.h"

/*******************************************************************************
* �� �� ��         : UART_HDMI_Init
* ��������		     : ����5��ʼ��
* ��    ��         : Bound�������ʣ�9600���� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void UART_HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef UART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��PORTC��PORTDʱ��
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //ʹ��UART5
	  USART_DeInit(UART_HDMI);  //��λ����5
	
	  //UART5_TX   PC.12
		GPIO_InitStructure.GPIO_Pin = UART_HDMI_TX; //PC.12
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
		GPIO_Init(UART_HDMI_TX_Port, &GPIO_InitStructure); //��ʼ��PC12
	  //UART5_RX	  PD.2
		GPIO_InitStructure.GPIO_Pin = UART_HDMI_RX;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
		GPIO_Init(UART_HDMI_RX_Port, &GPIO_InitStructure);  //��ʼ��PD2 

	  UART_InitStructure.USART_BaudRate = Bound;
		UART_InitStructure.USART_WordLength = USART_WordLength_8b;
		UART_InitStructure.USART_StopBits = USART_StopBits_1;
		UART_InitStructure.USART_Parity = USART_Parity_No;
		UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init(UART_HDMI, &UART_InitStructure);

		
		NVIC_InitStructure.NVIC_IRQChannel = UART_HDMI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= m_irq ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
		//USART_ITConfig(UART_HDMI, USART_IT_RXNE, ENABLE);//�����ж�
		USART_Cmd(UART_HDMI, ENABLE); 
}
/*******************************************************************************
* �� �� ��         : UART_HDMI_SendS
* ��������		     : �ַ������ͺ���
* ��    ��         : String:�ַ���
* ��    ��         : ��
*******************************************************************************/
void UART_HDMI_SendS(char *String)	
{
	while(*String != '\0')
	{
		USART_SendData(UART_HDMI,*String);  //����һ���ֽ�
		
		while(USART_GetFlagStatus(UART_HDMI,USART_FLAG_TXE)==RESET)
		{
			;
		};//�ȴ����ͽ���
		String++;
	}
}
/*******************************************************************************
* �� �� ��         : UART_HDMI_SendB
* ��������		     : �ֽڷ��ͺ�������������0xff������
* ��    ��         : Byte:�ֽ�
* ��    ��         : ��
*******************************************************************************/
void UART_HDMI_SendB(u8 Byte, u8 Tim)		
{		 
	u8 i;
	for(i=0;i<Tim;i++)
	{
		if(Byte!=0)
		{
			USART_SendData(UART_HDMI,Byte);  //����һ���ֽ�
			while(USART_GetFlagStatus(UART_HDMI,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
		}
		else 
		{
			return ;
		}
	} 
} 
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
