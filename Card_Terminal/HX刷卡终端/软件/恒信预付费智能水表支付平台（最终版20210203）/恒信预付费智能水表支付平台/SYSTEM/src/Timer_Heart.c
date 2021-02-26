/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Timer_Heart.h
 * 项目描述 ：定时器握手接口设计    
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Timer_Heart.h"
#include "stm32f10x_tim.h"

/*******************************************************************************
* 函 数 名         : TIM_Heart_Init
* 函数功能		     : 定时握手初始化
* 输    入         : 时间=(arr+1)*(psc+1)/72M,m_irq：抢占优先级（1），s_irq：响应优先级（0）
* 输    出         : 
*******************************************************************************/
void TIM_Heart_Init(u16 arr,u16 psc,u16 m_irq, u16 s_irq)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 
  NVIC_InitTypeDef NVIC_InitStructure;
	/* 开启定时器3时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);			//清除TIMx的中断待处理位:TIM 中断源
	TIM_TimeBaseInitStructure.TIM_Period = arr;			//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//设置用来作为TIMx时钟频率预分频值，100Khz计数频率
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;	 //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);	
	TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
	
		/* 设置中断参数，并打开中断 */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	//使能或者失能指定的TIM中断

	/* 设置NVIC参数 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//打开TIM3_IRQn的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=m_irq;	//抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=s_irq;  			//响应优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能
	NVIC_Init(&NVIC_InitStructure);	 
}

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
