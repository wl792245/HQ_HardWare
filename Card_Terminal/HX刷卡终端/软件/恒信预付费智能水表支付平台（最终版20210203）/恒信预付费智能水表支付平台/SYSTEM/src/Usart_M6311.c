/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Usart_M6311.c
 * ��Ŀ���� �������崮��2��M6311����      
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/

#include "stm32f10x_usart.h"
#include "Usart_M6311.h"
#include "string.h"
#include "systick.h"
#include "String_Get.h"
uint8_t  usart2_rcv_buf[MAX_RCV_LEN];
volatile uint32_t   usart2_rcv_len = 0;

/*******************************************************************************
* �� �� ��         : USART_M6311_Init
* ��������		     : ����2��ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void USART_M6311_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* config USART2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    /* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_M6311_TX;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_M6311_Port, &GPIO_InitStructure);
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_M6311_RX;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_M6311_Port, &GPIO_InitStructure);
    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = Bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART_M6311, &USART_InitStructure);
    USART_Cmd(USART_M6311, ENABLE);
    //Enable usart2 receive interrupt
    USART_ITConfig(USART_M6311, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART_M6311_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* �� �� ��         : USART_M6311_Write
* ��������		     : ͨ������2��M6311д����
* ��    ��         : Data���ַ����� len���ַ�������
* ��    ��         : ��
*******************************************************************************/
void USART_M6311_Write(char *Data, uint32_t len)
{
    uint32_t i;
    USART_ClearFlag(USART_M6311, USART_FLAG_TC);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART_M6311, *Data++);
        while( USART_GetFlagStatus(USART_M6311, USART_FLAG_TC) == RESET );
    }
}
/*******************************************************************************
* �� �� ��         : USART_M6311_SendCmd
* ��������		     : ͨ������2��M6311дָ��
* ��    ��         : Data���ַ����� len���ַ�������
* ��    ��         : ��
*******************************************************************************/
int  USART_M6311_SendCmd(char* cmd, char* result,  uint32_t timeOut)
{  
	int sendattime;
	int i = 0;
	sendattime=0;//50->5,20181022
	while(sendattime<3)
	{
		String_Clear((char *)usart2_rcv_buf, strlen((const char *)usart2_rcv_buf));
		usart2_rcv_len = 0;
		sendattime++;
		USART_M6311_Write(cmd, strlen((const char *)cmd));
		delay_ms(timeOut);   //�ȴ��������ݽ���
		for(i=0;i<20;i++)			//�ȴ��������ݽ���
		{
			if(usart2_rcv_len>0)
			{
				delay_ms(941);
				if((NULL != strstr((const char *)usart2_rcv_buf, (const char *)result)))
				{
						return 1;
				}
				usart2_rcv_len = 0;
			}
			delay_ms(341);
		}
  }
	return 0;
}



/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
