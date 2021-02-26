/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_HDMI.h
 * ��Ŀ���� �������崮��5��Դ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��PC12(TX)   PD2(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __UART_HDMI_H
#define __UART_HDMI_H

#include "stm32f10x.h"


#define UART_HDMI_TX_Port       GPIOC
#define UART_HDMI_RX_Port       GPIOD
#define UART_HDMI_TX            GPIO_Pin_12
#define UART_HDMI_RX            GPIO_Pin_2
#define UART_HDMI               UART5
#define UART_HDMI_IRQn          UART5_IRQn
#define UART_HDMI_IRQHandler    UART5_IRQHandler

//Һ����������9600����ռ���ȼ�3����Ӧ���ȼ�1
void UART_HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq);
void UART_HDMI_SendS(char *String);
void UART_HDMI_SendB(u8 Byte, u8 Tim);

#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
