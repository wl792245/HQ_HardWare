/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Timer_Deal.h
 * 项目描述 ：定时器处理函数声明
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/

#ifndef __TIMER_DEAL_H_
#define __TIMER_DEAL_H_

#include "stm32f10x.h"
#include "Timer_Heart.h"



extern volatile u32  sec;
extern volatile u32  err;
extern volatile uint32_t   TimeIsInter;

void FirstHeart_Open(char *TerminalId);
void TIM_Heart_Deal(void);






#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
