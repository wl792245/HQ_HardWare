/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Uart_XF.h
 * ��Ŀ���� �������崮��4��Դ����        
 * ����ƽ̨ ������STM32F103ZET6оƬ����
  * Ӳ�����ӣ�PC10(TX)   PC11(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __UART_XFS_H
#define __UART_XFS_H

#include "stm32f10x.h"


#define UART_XFS_Port          GPIOC
#define UART_XFS_TX            GPIO_Pin_10
#define UART_XFS_RX            GPIO_Pin_11
#define UART_XFS               UART4
#define UART_XFS_IRQn          UART4_IRQn
#define UART_XFS_IRQHandler    UART4_IRQHandler

void UART_XFS_Init(u32 Bound, u16 m_irq, u16 s_irq);
void UART_XFS_SendB(u8 Byte);
void UART_XFS_SendS(char *String, u16 len);
#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
