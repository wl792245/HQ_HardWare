/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：PAM8406_Config.h
 * 项目描述 ：功放模块调用     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：PAM8406(POWER)--PB4
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#ifndef __PAM8406_CONFIG_H
#define __PAM8406_CONFIG_H

#include "stm32f10x.h"


#define  PAM8406_POWER_PORT     GPIOB
#define  PAM8406_POWER_PIN      GPIO_Pin_4
#define  PAM8406_POWER_H        GPIO_SetBits(PAM8406_POWER_PORT,PAM8406_POWER_PIN)
#define  PAM8406_POWER_L        GPIO_ResetBits(PAM8406_POWER_PORT,PAM8406_POWER_PIN)

void PAM8406_Init(void);
void PAM8406_Close(void);
void PAM8406_Open(void);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
