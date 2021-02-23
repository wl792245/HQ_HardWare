/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Uart_HDMI.h
 * 项目描述 ：开发板串口3资源调用       
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：PB10(TX)   PB11(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-25
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

//液晶屏波特率9600，抢占优先级3，响应优先级1
void USART_HDMI_Init(u32 Bound);
void USART_HDMI_SendS(char *String);
void USART_HDMI_SendB(u8 Byte, u8 Tim);

#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
