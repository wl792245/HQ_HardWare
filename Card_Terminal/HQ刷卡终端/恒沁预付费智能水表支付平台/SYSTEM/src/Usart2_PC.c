/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Usart2_PC.h
 * ��Ŀ���� �������崮��2��Դ����       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��PA2(TX)   PA3(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-2-2
**********************************************************************************/
#include "stm32f10x_usart.h"
#include "Usart2_PC.h"

uint8_t  usart2_rcv_buf[MAX_RCV_LEN2];
volatile uint32_t   usart2_rcv_len = 0;


/*******************************************************************************
* �� �� ��         : USART_PC_Init
* ��������		     : ����2��ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void USART_PC_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
		GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* config USART2 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	   /* USART2 GPIO config */
    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_PC_TX;//2
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_PC_Port, &GPIO_InitStructure);
    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_PC_RX;//3
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_PC_Port, &GPIO_InitStructure);

    /* USART2 mode config */
    USART_InitStructure.USART_BaudRate = Bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART_PC, &USART_InitStructure);
    USART_Cmd(USART_PC, ENABLE);

    //Enable usart2 receive interrupt
    USART_ITConfig(USART_PC, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART_PC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq;//��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
}
/*******************************************************************************
* �� �� ��         : USART_PC_Write
* ��������		     : ͨ������2��PCд����
* ��    ��         : Data���ַ����� len���ַ�������
* ��    ��         : ��
*******************************************************************************/
void USART_PC_Write(char *Data, uint32_t len)
{
    uint32_t i;
    USART_ClearFlag(USART_PC, USART_FLAG_TC);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART_PC, *Data++);
        while( USART_GetFlagStatus(USART_PC, USART_FLAG_TC) == RESET );
    }
}



/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
