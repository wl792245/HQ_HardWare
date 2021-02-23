/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��HXCard_Config.h
 * ��Ŀ���� ������ģ�鹦������     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��HX(CS)--PA1
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
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
	char Value14428[2];//4428��������
	char SByStrHX[220];//������
	char HX4428Userid[17];//�û����
	char HX4428Sysnum[9];//������
}HXCard4428_Info;

extern volatile uint32_t   HXCardIsInter;

void HXCard_Init(u16 m_irq, u16 s_irq);

#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
