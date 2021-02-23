/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Usart_M6312.h
 * 项目描述 ：开发板串口1资源调用      
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：PA9(TX)   PA10(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#ifndef __USART_M6312_H
#define __USART_M6312_H

#include "stm32f10x.h"

#define MAX_RCV_LEN 1091

extern uint8_t  usart1_rcv_buf[MAX_RCV_LEN];
extern volatile uint32_t   usart1_rcv_len;

#define USART_M6312_Port          GPIOA
#define USART_M6312_TX            GPIO_Pin_9
#define USART_M6312_RX            GPIO_Pin_10
#define USART_M6312               USART1
#define USART_M6312_IRQn          USART1_IRQn
#define USART_M6312_IRQHandler    USART1_IRQHandler



void USART_M6312_Init(u32 Bound, u16 m_irq, u16 s_irq);
int  USART_M6312_SendCmd(char* cmd, char* result,  uint32_t timeOut);
void USART_M6312_Write(char *Data, uint32_t len);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
