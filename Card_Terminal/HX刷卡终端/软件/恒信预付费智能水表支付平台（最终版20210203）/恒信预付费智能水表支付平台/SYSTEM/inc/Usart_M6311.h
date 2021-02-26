/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Usart_M6311.h
 * ��Ŀ���� �������崮��2��Դ����      
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��PA2(TX)   PA3(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#ifndef __USART_M6311_H
#define __USART_M6311_H

#include "stm32f10x.h"

#define MAX_RCV_LEN 1091

extern uint8_t  usart2_rcv_buf[MAX_RCV_LEN];
extern volatile uint32_t   usart2_rcv_len;

#define USART_M6311_Port          GPIOA
#define USART_M6311_TX            GPIO_Pin_2
#define USART_M6311_RX            GPIO_Pin_3
#define USART_M6311               USART2
#define USART_M6311_IRQn          USART2_IRQn
#define USART_M6311_IRQHandler    USART2_IRQHandler



void USART_M6311_Init(u32 Bound, u16 m_irq, u16 s_irq);
int  USART_M6311_SendCmd(char* cmd, char* result,  uint32_t timeOut);
void USART_M6311_Write(char *Data, uint32_t len);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/

