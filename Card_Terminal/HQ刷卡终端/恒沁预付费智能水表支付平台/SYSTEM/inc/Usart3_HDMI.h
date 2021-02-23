/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_HDMI.h
 * ��Ŀ���� �������崮��3��Դ����       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��PB10(TX)   PB11(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-25
**********************************************************************************/
#ifndef __USART_HDMI_H
#define __USART_HDMI_H

#include "stm32f10x.h"


#define USART_HDMI_Port          GPIOB
#define USART_HDMI_TX            GPIO_Pin_10
#define USART_HDMI_RX            GPIO_Pin_11
#define USART_HDMI               USART3
#define USART_HDMI_IRQn          USART3_IRQn
#define USART_HDMI_IRQHandler    USART3_IRQHandler

//Һ����������9600����ռ���ȼ�3����Ӧ���ȼ�1
void USART_HDMI_Init(u32 Bound);
void USART_HDMI_SendS(char *String);
void USART_HDMI_SendB(u8 Byte, u8 Tim);

#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
