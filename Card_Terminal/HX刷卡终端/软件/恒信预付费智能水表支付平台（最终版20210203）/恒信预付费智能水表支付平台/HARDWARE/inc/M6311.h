/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：M6311.h
 * 项目描述 ：M6311模块调用     
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：M6311_TX-PA3  M6311_RX-PA2
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#ifndef __M6311_H
#define __M6311_H

#include "stm32f10x.h"
#include "Usart_M6311.h"
#include "AT24C02.h"

#define  M6311_POWER_PORT     GPIOC
#define  M6311_POWER_PIN      GPIO_Pin_4
#define  M6311_POWER_H        GPIO_SetBits(M6311_POWER_PORT,M6311_POWER_PIN)
#define  M6311_POWER_L        GPIO_ResetBits(M6311_POWER_PORT,M6311_POWER_PIN)

extern  unsigned char M6311_Signal;
extern  unsigned char M6311_Connecting;
extern  volatile unsigned char M6311_RestartFlag;

void M6311_Init(u32 Bound, u16 m_irq, u16 s_irq);
void M6311_Connect(pDeviceInit pServerInfo, char *ShowInfo);
void M6311_SendData(char * buf,uint32_t len);
void M6311_USART_GetRcvData(char *buf, uint32_t rcv_len);
void M6311_SignalQuery(char *Signal);
u8 M6311_UploadData(char * pdest,char *psrc, char *txt, int times,int len);

#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
