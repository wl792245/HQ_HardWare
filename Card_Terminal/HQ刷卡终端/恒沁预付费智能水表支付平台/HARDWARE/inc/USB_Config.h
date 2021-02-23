/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：USB_Config.h
 * 项目描述 ：USB外部接口调用       
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：USBDM-PA11 USBDP-PA12
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#ifndef __USB_CONFIG_H_
#define __USB_CONFIG_H_

#include "stm32f10x.h"
#include "hw_config.h"

void USB_CONFIG_Init(u16 m_irq, u16 s_irq);
void USB_PC_SendData(char * buf);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
