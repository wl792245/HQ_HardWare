/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：BSP_Config.h
 * 项目描述 ：板上硬件接口调用       
 * 开发平台 ：基于STM32F103C8T6芯片开发
 * 硬件连接 ：
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2020-10-29
**********************************************************************************/
#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

//**************自带库头文件********************//
#include "sys.h"
#include "stm32f10x_tim.h"
//**************自编库头文件********************//
#include "CardInfoDeal.h"
#include "AT24C02.h"
#include "HDMI_T050.h"
#include "M6311.h"
#include "Timer_Deal.h"
#include "XFS.h"
#include "systick.h"
#include "sys.h"
#include "string.h"
#include "IWDG.h"
#include "Timer_Heart.h"
#include "String_Get.h"
//**************外部变量调用声明****************//

//**************自写函数声明********************//
void BSP_Init(void);

#endif

/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************/
