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
* �� �� ��         : Card57_IRQHandler
* ��������		     : 57������ʱ�����ⲿ�ж�2
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card57_IRQHandler(void)
{

	if(EXTI_GetITStatus(Card57_Line)==SET)
	{
		delay_ms(100);
		if(EXTI_GetITStatus(Card57_Line)==SET)
		{
			EXTI_ClearITPendingBit(Card57_Line);//���EXTI��·����λ
			if(GPIO_ReadInputDataBit(Card57_GPIO,Card57_ICSW)==Bit_RESET)//ȷ������������
			{
				if(M6311_Connecting == 1)
				{
					XFS_Only_Play("���������У����Ժ�");
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
* �� �� ��         : Card_IRQHandler
* ��������		     : 24��4442������ʱ�����ⲿ�ж�9
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card_IRQHandler(void)
{
	Card24_LadderInfo   Info24ladder1;      //24��������Ϣ�ṹ��
	if(EXTI_GetITStatus(Card_Line)==SET)
	{
		delay_ms(500);       //��������
		if(EXTI_GetITStatus(Card_Line)==SET)
		{
			EXTI_ClearITPendingBit(Card_Line);	//���EXTI��·����λ	
			if(GPIO_ReadInputDataBit(Card_GPIO,Card_ICSW)==Bit_RESET)
			{
				  Card24_LadderReadCID(&Info24ladder1);   	//��24���ݿ�
					if(Info24ladder1.cflag==0xC4)       //������IP�޸Ŀ�
					{
						if (CopyIPPortTo24C02())                 //�����ÿ��ķ�������ַ���Ƶ��������ϵ�24c02
						{
							XFS_HDMI_Play("������IP����ʧ�ܣ�������");
						}
						if (CopyQRcodeTo24C02())               //�����ÿ��Ķ�ά�븴�Ƶ��������ϵ�24c02
						{
							XFS_HDMI_Play("��ά�����ʧ�ܣ�������");
						}
						HDMIShowChangeSuccess();
						XFS_Only_Play("IP��ά����ĳɹ�");
						AT24C02IPTid_Init();
						delay_ms(1000);
						CPU_Reset();
					}
					else if ((Info24ladder1.cflag!=0xC4)&&(M6311_Connecting == 1))
					{
						XFS_Only_Play("���������У����Ժ�");
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
* �� �� ��         : USART_M6311_IRQHandler
* ��������		     : ����2��������M6311������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USART_M6311_IRQHandler(void)
{
	unsigned char data;
	if(USART_GetFlagStatus(USART_M6311,USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(USART_M6311,USART_FLAG_ORE);
	}//���������ʱ���ִ��󣬸ñ�־λ�ͻ���λ����������������в���Ԥ֪�Ĵ���
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
* �� �� ��         : TIM3_IRQHandler
* ��������		     : �������ֲ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//����Ƿ����ж�	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );	//����жϱ�־
	}
	//IWDG_Feed();
	sec++;
	if(sec >= 600)  //10��������һ��
	{
		sec=0;
		TimeIsInter = 1;
	}
	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����	
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
