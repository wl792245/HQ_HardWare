/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：HXCard_Config.c
 * 项目描述 ：华旭卡模块功能定义     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：HX(CS)--PA1
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-4
**********************************************************************************/
#include "HXCard_Config.h"
#include "stm32f10x_exti.h"

volatile uint32_t   HXCardIsInter=0; //4428卡中断标志位
/*******************************************************************************
* 函 数 名         : HXCard_Init
* 函数功能		     : 华旭卡模块初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HXCard_Init(u16 m_irq, u16 s_irq)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
		//配置gpio，华旭止插卡中断,PA1
		GPIO_InitStructure.GPIO_Pin=HXCARD_INIT_CSW;	 //选择你要设置的IO口(pin_1)
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	  //设置上拉输入模式
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
		GPIO_Init(HXCARD_INIT_Port,&GPIO_InitStructure); /* 初始化GPIO */	
		/*将GPIO管脚与外部中断线连接*/
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);//选择GPIO管脚用作外部中断线路
		/*配置EXTI*/
		EXTI_InitStructure.EXTI_Line=HXCARD_Line;
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//下降沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure); 
		
		/* 设置NVIC参数 */	
		NVIC_InitStructure.NVIC_IRQChannel = HXCARD_INIT_IRQn;		//打开EXTI1的全局中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //抢占优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //响应优先级为2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //使能
		NVIC_Init(&NVIC_InitStructure);	
}








/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
