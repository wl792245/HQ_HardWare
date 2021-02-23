/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��USB_Config.h
 * ��Ŀ���� ��USB�ⲿ�ӿڵ���       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��USBDM-PA11 USBDP-PA12
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#ifndef __USB_CONFIG_H_
#define __USB_CONFIG_H_

#include "stm32f10x.h"
#include "hw_config.h"

void USB_CONFIG_Init(u16 m_irq, u16 s_irq);
void USB_PC_SendData(char * buf);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
