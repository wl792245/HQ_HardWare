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
* �� �� ��         : HXCARD_INIT_IRQHandler
* ��������		     : �����ⲿ�жϺ�����4428��4442������ʱ�����ⲿ�жϣ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HXCARD_INIT_IRQHandler(void)
{	
		if(EXTI_GetITStatus(HXCARD_Line)==SET)
		{
			delay_ms(100);
			if(EXTI_GetITStatus(HXCARD_Line)==SET)
			{
				EXTI_ClearITPendingBit(HXCARD_Line);//���EXTI��·����λ
				if(GPIO_ReadInputDataBit(HXCARD_INIT_Port,HXCARD_INIT_CSW)==Bit_RESET)//ȷ������������
				{
					if(M6312_Connecting == 1)
					{
						HDMIShowInfo("���������У����Ժ�");
					}
					else if (PC_Connecting == 1)
					{
						HDMIShowInfo("���������У����Ժ�");
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
* �� �� ��         : SCCARD_INIT_IRQHandler
* ��������		     : �����ⲿ�жϺ�����5557������ʱ�����ⲿ�жϣ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void SCCARD_INIT_IRQHandler(void)
{	
		if(EXTI_GetITStatus(SCCARD_Line)==SET)
		{
			delay_ms(100);
			if(EXTI_GetITStatus(SCCARD_Line)==SET)
			{
				EXTI_ClearITPendingBit(SCCARD_Line);//���EXTI��·����λ
				if(GPIO_ReadInputDataBit(SCCARD_INIT_Port,SCCARD_INIT_CSW)==Bit_RESET)//ȷ������������
				{
					if(M6312_Connecting == 1)
					{
						HDMIShowInfo("���������У����Ժ�");
					}
					else if (PC_Connecting == 1)
					{
						HDMIShowInfo("���������У����Ժ�");
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
* �� �� ��         : USART_M6312_IRQHandler
* ��������		     : ����1��������M6312������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USART1_IRQHandler(void)
{
	unsigned char data;
	
	if(USART_GetFlagStatus(USART_M6312,USART_FLAG_ORE) != RESET)//�ж���������־λ
	{
		USART_ClearFlag(USART_M6312,USART_FLAG_ORE);//��������жϣ���λ�Ĵ�����ǰ�����ݽ������ǣ������յ����ݽ���ʧ
	}
	USART_ITConfig(USART_M6312, USART_IT_RXNE, DISABLE);//�жϱ�־λ����
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
* �� �� ��         : USART_PC_IRQHandler
* ��������		     : ����2��������PC������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USART_PC_IRQHandler(void)
{
	unsigned char data;
	if(USART_GetFlagStatus(USART_PC,USART_FLAG_ORE) != RESET)
	{
		USART_ClearFlag(USART_PC,USART_FLAG_ORE);//��������жϣ���λ�Ĵ�����ǰ�����ݽ������ǣ������յ����ݽ���ʧ
	}//���������ʱ���ִ��󣬸ñ�־λ�ͻ���λ����������������в���Ԥ֪�Ĵ���
	USART_ITConfig(USART_PC, USART_IT_RXNE, DISABLE);//�����жϱ�־
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
* �� �� ��         : TIM3_IRQHandler
* ��������		     : �������ֲ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void TIM3_IRQHandler()   
{
	TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)	//����Ƿ����ж�	
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );	//����жϱ�־
	}
	Server_Time++;
	PC_Time++;
	if(Server_Time >= 600)  //10���������������һ��
	{
		Server_Time=0;
		Server_TimeIsInter = 1;
	}
	if(PC_Time>= 120)      //2�������������һ��
	{
		PC_Time = 0;
		PC_TimeIsInter = 1;
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

//���Ź��жϴ������
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
