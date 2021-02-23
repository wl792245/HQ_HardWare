/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Usart2_PC.h
 * ��Ŀ���� �������崮��2��Դ����       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��PB10(TX)   PB11(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-2-2
**********************************************************************************/
#ifndef __USART_PC_H
#define __USART_PC_H

#include "stm32f10x.h"

#define MAX_RCV_LEN2 1091

#define USART_PC_Port          GPIOA
#define USART_PC_TX            GPIO_Pin_2
#define USART_PC_RX            GPIO_Pin_3
#define USART_PC               USART2
#define USART_PC_IRQn          USART2_IRQn
#define USART_PC_IRQHandler    USART2_IRQHandler

extern uint8_t  usart2_rcv_buf[MAX_RCV_LEN2];
extern volatile uint32_t   usart2_rcv_len;

//Һ����������9600����ռ���ȼ�0����Ӧ���ȼ�1
void USART_PC_Init(u32 Bound, u16 m_irq, u16 s_irq);
void USART_PC_Write(char *Data, uint32_t len);

#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
