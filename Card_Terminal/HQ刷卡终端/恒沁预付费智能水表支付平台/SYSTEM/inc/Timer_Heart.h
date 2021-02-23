/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Timer_Heart.h
 * 项目描述 ：定时器握手接口声明    
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#ifndef __TIMER_HEART_H_
#define __TIMER_HEART_H_

#include "stm32f10x.h"



void TIM_Heart_Init(u16 arr,u16 psc,u16 m_irq, u16 s_irq);


#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
