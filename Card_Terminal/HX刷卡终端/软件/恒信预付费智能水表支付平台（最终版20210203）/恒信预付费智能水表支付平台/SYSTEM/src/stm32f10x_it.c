/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"

#include "M6311.h"
#include "CardInfoDeal.h"
#include "XFS.h"
#include "HDMI_T050.h"
#include "AT24C02.h"
#include "systick.h"
#include "String_Get.h"
#include "IWDG.h"
#include "Timer_Deal.h"
/*******************************************************************************
* 函 数 名         : Card57_IRQHandler
* 函数功能		     : 57卡插入时触发外部中断2
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card57_IRQHandler(void)
{

	if(EXTI_GetITStatus(Card57_Line)==SET)
	{
		delay_ms(100);
		if(EXTI_GetITStatus(Card57_Line)==SET)
		{
			EXTI_ClearITPendingBit(Card57_Line);//清除EXTI线路挂起位
			if(GPIO_ReadInputDataBit(Card57_GPIO,Card57_ICSW)==Bit_RESET)//确定卡物理在线
			{
				if(M6311_Connecting == 1)
				{
					XFS_Only_Play("网络连接中，请稍后");
				}
				else
				{
					Card57_Flag = 1;
				}		  
			}
		}
	}
}
/*******************************************************************************
* 函 数 名         : Card_IRQHandler
* 函数功能		     : 24，4442卡插入时触发外部中断9
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card_IRQHandler(void)
{
	Card24_LadderInfo   Info24ladder1;      //24卡阶梯信息结构体
	if(EXTI_GetITStatus(Card_Line)==SET)
	{
		delay_ms(500);       //消抖处理
		if(EXTI_GetITStatus(Card_Line)==SET)
		{
			EXTI_ClearITPendingBit(Card_Line);	//清除EXTI线路挂起位	
			if(GPIO_ReadInputDataBit(Card_GPIO,Card_ICSW)==Bit_RESET)
			{
				  Card24_LadderReadCID(&Info24ladder1);   	//读24阶梯卡
					if(Info24ladder1.cflag==0xC4)       //服务器IP修改卡
					{
						if (CopyIPPortTo24C02())                 //将配置卡的服务器地址复制到开发板上的24c02
						{
							XFS_HDMI_Play("服务器IP更改失败，请重试");
						}
						if (CopyQRcodeTo24C02())               //将配置卡的二维码复制到开发板上的24c02
						{
							XFS_HDMI_Play("二维码更改失败，请重试");
						}
						HDMIShowChangeSuccess();
						XFS_Only_Play("IP二维码更改成功");
						AT24C02IPTid_Init();
						delay_ms(1000);
						CPU_Reset();
					}
					else if ((Info24ladder1.cflag!=0xC4)&&(M6311_Connecting == 1))
					{
						XFS_Only_Play("网络连接中，请稍后");
					}
					else
					{
						CardIsInter = 1;
					}
					
			}
		}	
	}
}	
/*******************************************************************************
* 函 数 名         : USART_M6311_IRQHandler
* 函数功能		     : 串口2接收来自M6311的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART_M6311_IRQHandler(void)
{
	unsigned char data;
	if(USART_GetFlagStatus(USART_M6311,USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(USART_M6311,USART_FLAG_ORE);
	}//当串口溢出时出现错误，该标志位就会置位，必须清掉。否则有不可预知的错误。
	USART_ITConfig(USART_M6311, USART_IT_RXNE, DISABLE);
	if(USART_M6311->SR & 0x0F)
	{
		data = USART_M6311->DR;
	}
	else if(USART_M6311->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
	{		//GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
		data = USART_M6311->DR;
		usart2_rcv_buf[usart2_rcv_len++]=data;
	}
	else
	{
		;
	}
	USART_ITConfig(USART_M6311, USART_IT_RXNE, ENABLE);
}
	
/*******************************************************************************
* 函 数 名         : TIM3_IRQHandler
* 函数功能		     : 进行握手测试
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//检查是否发生中断	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );	//清除中断标志
	}
	//IWDG_Feed();
	sec++;
	if(sec >= 600)  //10分钟握手一次
	{
		sec=0;
		TimeIsInter = 1;
	}
	
	TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设	
} 

void NMI_Handler(void)
{
}
void HardFault_Handler(void)
{
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1)
	{

	}
}
void MemManage_Handler(void)
{
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}
void BusFault_Handler(void)
{
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}
void UsageFault_Handler(void)
{
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}
void SVC_Handler(void)
{
}
void DebugMon_Handler(void)
{
}
void PendSV_Handler(void)
{
}
void SysTick_Handler(void)
{
}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
