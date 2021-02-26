/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：M6312.h
 * 项目描述 ：M6312模块调用     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：M6312(RX)--PA9 M6312(TX)--PA10 M6312(POWER)--PA15
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#ifndef __M6312_H
#define __M6312_H

#include "stm32f10x.h"
#include "Usart1_M6312.h"
#include "AT24C02.h"

#define  M6312_POWER_PORT     GPIOA
#define  M6312_POWER_PIN      GPIO_Pin_15
#define  M6312_POWER_H        GPIO_SetBits(M6312_POWER_PORT,M6312_POWER_PIN)
#define  M6312_POWER_L        GPIO_ResetBits(M6312_POWER_PORT,M6312_POWER_PIN)

extern volatile unsigned char M6312_Signal;
extern volatile unsigned char M6312_Connecting;
extern volatile unsigned char M6312_RestartFlag;
void M6312_Init(u32 Bound, u16 m_irq, u16 s_irq);
void M6312_Connect(pDeviceInit pServerInfo,char *ShowInfo);
void M6312_SendData(char * buf,uint32_t len);
void M6312_USART_GetRcvData(char *buf, uint32_t rcv_len);
unsigned char M6312_SignalQuery(char *Signal);
unsigned char M6312_TimeQuery(PDataInit TimeInit);
u8 M6312_UploadData(char * pdest,char *psrc, char *txt, int times,int len);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
