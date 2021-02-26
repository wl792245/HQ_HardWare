/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��XFS.h
 * ��Ŀ���� ������ģ�����        
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ�����ӣ�POWER-PC5 XFS_RX-PC10(TX)   XFS_TX-PC11(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __XFS_H
#define __XFS_H

#include "stm32f10x.h"
#include "Uart_XFS.h"

#define XFS_Port          GPIOC
#define XFS_POWER         GPIO_Pin_5

#define XFS_POWER_H       GPIO_SetBits(XFS_Port,XFS_POWER)	  //��������Դ
#define XFS_POWER_L       GPIO_ResetBits(XFS_Port,XFS_POWER)	//��������Դ

void XFS_Init(u32 Bound, u16 m_irq, u16 s_irq);
void XFS_HDMI_Play(char *CQdata);
void XFS_Only_Play(char *CQdata);
void XFS_Success_Play(char *CQdata,char *RechargeSucessValue,int Unit);
void XFS_Fail_Play(char *CQdata);
#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
