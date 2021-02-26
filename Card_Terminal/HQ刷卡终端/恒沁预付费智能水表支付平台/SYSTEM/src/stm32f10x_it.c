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
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"
#include "M6312.h"
#include "PC_Config.h"
#include "Timer_Deal.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"
#include "HDMI_T050.h"
#include "systick.h" 
/*******************************************************************************
* 函 数 名         : HXCARD_INIT_IRQHandler
* 函数功能		     : 华旭外部中断函数（4428、4442卡插入时触发外部中断）
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void HXCARD_INIT_IRQHandler(void)
{	
		if(EXTI_GetITStatus(HXCARD_Line)==SET)
		{
			delay_ms(100);
			if(EXTI_GetITStatus(HXCARD_Line)==SET)
			{
				EXTI_ClearITPendingBit(HXCARD_Line);//清除EXTI线路挂起位
				if(GPIO_ReadInputDataBit(HXCARD_INIT_Port,HXCARD_INIT_CSW)==Bit_RESET)//确定卡物理在线
				{
					if(M6312_Connecting == 1)
					{
						HDMIShowInfo("网络连接中，请稍后");
					}
					else if (PC_Connecting == 1)
					{
						HDMIShowInfo("电脑连接中，请稍后");
					}
					else
					{
						HXCardIsInter = 1;
					}
				}
			}
		}
}
/*******************************************************************************
* 函 数 名         : SCCARD_INIT_IRQHandler
* 函数功能		     : 三川外部中断函数（5557卡插入时触发外部中断）
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SCCARD_INIT_IRQHandler(void)
{	
		if(EXTI_GetITStatus(SCCARD_Line)==SET)
		{
			delay_ms(100);
			if(EXTI_GetITStatus(SCCARD_Line)==SET)
			{
				EXTI_ClearITPendingBit(SCCARD_Line);//清除EXTI线路挂起位
				if(GPIO_ReadInputDataBit(SCCARD_INIT_Port,SCCARD_INIT_CSW)==Bit_RESET)//确定卡物理在线
				{
					if(M6312_Connecting == 1)
					{
						HDMIShowInfo("网络连接中，请稍后");
					}
					else if (PC_Connecting == 1)
					{
						HDMIShowInfo("电脑连接中，请稍后");
					}
					else
					{
						SCCardIsInter = 1;
					}
				}
			}
		}
}

/*******************************************************************************
* 函 数 名         : USART_M6312_IRQHandler
* 函数功能		     : 串口1接收来自M6312的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART1_IRQHandler(void)
{
	unsigned char data;
	
	if(USART_GetFlagStatus(USART_M6312,USART_FLAG_ORE) != RESET)//判断溢出错误标志位
	{
		USART_ClearFlag(USART_M6312,USART_FLAG_ORE);//溢出错误中断，移位寄存器以前的内容将被覆盖，随后接收的数据将丢失
	}
	USART_ITConfig(USART_M6312, USART_IT_RXNE, DISABLE);//中断标志位清零
	if(USART_M6312->SR & 0x0F)
	{
		data = USART_M6312->DR;
	}
	else if(USART_M6312->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
	{		//GPIO_SetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
		data = USART_M6312->DR;
		usart1_rcv_buf[usart1_rcv_len++]=data;
	}
	else
	{
		;
	}		
	USART_ITConfig(USART_M6312, USART_IT_RXNE, ENABLE);
}
/*******************************************************************************
* 函 数 名         : USART_PC_IRQHandler
* 函数功能		     : 串口2接收来自PC的数据
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART_PC_IRQHandler(void)
{
	unsigned char data;
	if(USART_GetFlagStatus(USART_PC,USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(USART_PC,USART_FLAG_ORE);//溢出错误中断，移位寄存器以前的内容将被覆盖，随后接收的数据将丢失
	}//当串口溢出时出现错误，该标志位就会置位，必须清掉。否则有不可预知的错误。
	USART_ITConfig(USART_PC, USART_IT_RXNE, DISABLE);//接收中断标志
	if(USART_PC->SR & 0x0F)
	{
		data = USART_PC->DR;
	}
	else if(USART_PC->SR & USART_FLAG_RXNE)   //Receive Data Reg Full Flag
	{
		data = USART_PC->DR;
		usart2_rcv_buf[usart2_rcv_len++]=data;
	}
	else
	{
		;
	}
	USART_ITConfig(USART_PC, USART_IT_RXNE, ENABLE);

}
/*******************************************************************************
* 函 数 名         : TIM3_IRQHandler
* 函数功能		     : 进行握手测试
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void TIM3_IRQHandler()   
{
	TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//检查是否发生中断	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );	//清除中断标志
	}
	Server_Time++;
	PC_Time++;
	if(Server_Time >= 600)  //10分钟与服务器握手一次
	{
		Server_Time=0;
		Server_TimeIsInter = 1;
	}
	if(PC_Time>= 120)      //2分钟与电脑握手一次
	{
		PC_Time = 0;
		PC_TimeIsInter = 1;
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

//看门狗中断处理程序
void WWDG_IRQHandler(void)
{
	if (WWDG->SR&=0x01)
	{
	  WWDG->SR=0X00;
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
