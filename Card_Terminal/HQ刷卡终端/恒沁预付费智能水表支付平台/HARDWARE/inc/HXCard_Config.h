/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：HXCard_Config.h
 * 项目描述 ：华旭卡模块功能声明     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：HX(CS)--PA1
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#ifndef __HXCARD_CONFIG_H_
#define __HXCARD_CONFIG_H_

#include "stm32f10x.h"


#define HXCARD_INIT_Port          GPIOA
#define HXCARD_INIT_CSW           GPIO_Pin_1
#define HXCARD_Line               EXTI_Line1
#define HXCARD_INIT_IRQn          EXTI1_IRQn
#define HXCARD_INIT_IRQHandler    EXTI1_IRQHandler


typedef struct HXCARD4428
{
	char Value14428[2];//4428卡的类型
	char SByStrHX[220];//卡内容
	char HX4428Userid[17];//用户编号
	char HX4428Sysnum[9];//区域码
}HXCard4428_Info;

extern volatile uint32_t   HXCardIsInter;

void HXCard_Init(u16 m_irq, u16 s_irq);

#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
