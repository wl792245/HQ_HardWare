/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：XFS.h
 * 项目描述 ：语音模块调用        
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接：POWER-PC5 XFS_RX-PC10(TX)   XFS_TX-PC11(RX)
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#ifndef __XFS_H
#define __XFS_H

#include "stm32f10x.h"
#include "Uart_XFS.h"

#define XFS_Port          GPIOC
#define XFS_POWER         GPIO_Pin_5

#define XFS_POWER_H       GPIO_SetBits(XFS_Port,XFS_POWER)	  //开语音电源
#define XFS_POWER_L       GPIO_ResetBits(XFS_Port,XFS_POWER)	//关语音电源

void XFS_Init(u32 Bound, u16 m_irq, u16 s_irq);
void XFS_HDMI_Play(char *CQdata);
void XFS_Only_Play(char *CQdata);
void XFS_Success_Play(char *CQdata,char *RechargeSucessValue,int Unit);
void XFS_Fail_Play(char *CQdata);
#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
