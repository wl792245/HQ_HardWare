/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Usart2_PC.h
 * 项目描述 ：开发板串口2资源调用       
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：PB10(TX)   PB11(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-2-2
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

//液晶屏波特率9600，抢占优先级0，响应优先级1
void USART_PC_Init(u32 Bound, u16 m_irq, u16 s_irq);
void USART_PC_Write(char *Data, uint32_t len);

#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
