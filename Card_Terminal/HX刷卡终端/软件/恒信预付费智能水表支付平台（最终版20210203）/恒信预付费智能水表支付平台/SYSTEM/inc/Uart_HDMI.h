/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Uart_HDMI.h
 * 项目描述 ：开发板串口5资源调用       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：PC12(TX)   PD2(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
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

//液晶屏波特率9600，抢占优先级3，响应优先级1
void UART_HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq);
void UART_HDMI_SendS(char *String);
void UART_HDMI_SendB(u8 Byte, u8 Tim);

#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
