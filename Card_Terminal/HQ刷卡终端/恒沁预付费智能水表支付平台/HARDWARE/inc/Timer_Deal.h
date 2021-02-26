/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Timer_Deal.h
 * ��Ŀ���� ����ʱ������������
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/

#ifndef __TIMER_DEAL_H_
#define __TIMER_DEAL_H_

#include "stm32f10x.h"
#include "Timer_Heart.h"



extern volatile u32  Server_Time;  //�����������ʱ��
extern volatile u32  PC_Time;      //���������ʱ��
extern volatile u32  err;
extern volatile uint32_t   Server_TimeIsInter;
extern volatile uint32_t   PC_TimeIsInter;

void FirstHeart_Open(char *TerminalId);
void TIM_Heart_Deal(void);






#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
