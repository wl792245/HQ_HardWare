/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Usart_M6312.c
 * ��Ŀ���� �������崮��1��M6312����     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#include "stm32f10x_usart.h"
#include "Usart1_M6312.h"
#include "string.h"
#include "systick.h"
#include "String_Config.h"


#if 1
#include "stdio.h"
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

uint8_t  usart1_rcv_buf[MAX_RCV_LEN];
volatile uint32_t   usart1_rcv_len = 0;

/*******************************************************************************
* �� �� ��         : USART_M6312_Init
* ��������		     : ����1��ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void USART_M6312_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART1 Tx (PA.9) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USART_M6312_TX;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_M6312_Port, &GPIO_InitStructure);
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = USART_M6312_RX;
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
    GPIO_Init(USART_M6312_Port, &GPIO_InitStructure);
    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = Bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    NVIC_InitStructure.NVIC_IRQChannel = USART_M6312_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
		
		USART_Init(USART_M6312, &USART_InitStructure);
    //Enable usart1 receive interrupt
    USART_ITConfig(USART_M6312, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART_M6312, ENABLE);
}

/*******************************************************************************
* �� �� ��         : USART_M6312_Write
* ��������		     : ͨ������1��M6312д����
* ��    ��         : Data���ַ����� len���ַ�������
* ��    ��         : ��
*******************************************************************************/
void USART_M6312_Write(char *Data, uint32_t len)
{
    uint32_t i;
    USART_ClearFlag(USART_M6312, USART_FLAG_TC);
    for(i = 0; i < len; i++)
    {
        USART_SendData(USART_M6312, *Data++);
        while( USART_GetFlagStatus(USART_M6312, USART_FLAG_TC) == RESET );
    }
}
/*******************************************************************************
* �� �� ��         : USART_M6312_SendCmd
* ��������		     : ͨ������1��M6312дָ��
* ��    ��         : Data���ַ����� len���ַ�������
* ��    ��         : 1:�ɹ��� 0��ʧ��
*******************************************************************************/
int  USART_M6312_SendCmd(char* cmd, char* result,  uint32_t timeOut)
{  
	int sendattime = 49;
	int i = 0;
	int j;
	while(i<sendattime)
	{
		String_Clear((char *)usart1_rcv_buf, strlen((const char *)usart1_rcv_buf));
		usart1_rcv_len = 0;
		USART_M6312_Write(cmd, strlen((const char *)cmd));
		delay_ms(timeOut);   //�ȴ��������ݽ���
		for(j=0;j<20;j++)			//�ȴ��������ݽ���
		{
			if(usart1_rcv_len>0)
			{
				delay_ms(941);
				if((NULL != strstr((const char *)usart1_rcv_buf, (const char *)result)))
				{
						delay_ms(200);
						return 1;
				}
				usart1_rcv_len = 0;
			}
			delay_ms(341);
		}
		i++;	
  }
	return 0;
}

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
