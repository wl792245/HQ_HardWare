/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：BSP_Config.h
 * 项目描述 ：板上硬件接口调用       
 * 开发平台 ：基于STM32F103C8T6芯片开发
 * 硬件连接 ：
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-01-25
**********************************************************************************/
#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#include "M6312.h"
#include "AT24C02.h"
#include "USB_Config.h"
#include "PC_Config.h"
#include "PAM8406_Config.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"
#include "HDMI_T050.h"
#include "Timer_Deal.h"
#include "sys.h"
#include "systick.h"
#include "CardInfoDeal.h"
#include "String_Config.h"

void BSP_Init(void);
void Remote_ChangeIp(void);
#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
