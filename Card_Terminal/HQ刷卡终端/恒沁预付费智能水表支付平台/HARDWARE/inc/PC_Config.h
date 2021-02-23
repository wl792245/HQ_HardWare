/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��PC_Config.h
 * ��Ŀ���� ��PCģ�����     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��PC(POWER)--PA7
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#ifndef __PC_CONFIG_H
#define __PC_CONFIG_H

#include "stm32f10x.h"
#include "Usart2_PC.h"
#include "AT24C02.h"

#define  PC_POWER_PORT     GPIOA
#define  PC_POWER_PIN      GPIO_Pin_7
#define  PC_POWER_H        GPIO_SetBits(PC_POWER_PORT,PC_POWER_PIN)
#define  PC_POWER_L        GPIO_ResetBits(PC_POWER_PORT,PC_POWER_PIN)

extern volatile unsigned char PC_Connecting;



void PC_Init(u32 Bound, u16 m_irq, u16 s_irq);
void PC_Close(void);
void PC_Open(void);
void PC_Start(void);
void PC_ShutDown(void);
void PC_ReadCard(void);
void PC_WriteCard(const char *psrc, CardType_Info CardTypeInfo);
void PC_StartPam(char *txt);
void PC_SendData(char * data);
u8 PC_ReceiveData(char * pdest, int len);
void PC_Usart_GetRcvData(char * buf, int rcv_len);
void PC_USB_GetRcvData(char * buf, int rcv_len);
u8 PC_ReadData(char * pdest,  int times,int len);
u8 PC_WirteData(char * pdest, const char *psrc, int times,int len, CardType_Info CardTypeInfo);

void USART_PC_SendData(char * buf,uint32_t len);

#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
