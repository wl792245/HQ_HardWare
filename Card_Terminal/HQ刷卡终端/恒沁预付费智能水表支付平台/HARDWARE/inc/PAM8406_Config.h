/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��PAM8406_Config.h
 * ��Ŀ���� ������ģ�����     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��PAM8406(POWER)--PB4
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#ifndef __PAM8406_CONFIG_H
#define __PAM8406_CONFIG_H

#include "stm32f10x.h"


#define  PAM8406_POWER_PORT     GPIOB
#define  PAM8406_POWER_PIN      GPIO_Pin_4
#define  PAM8406_POWER_H        GPIO_SetBits(PAM8406_POWER_PORT,PAM8406_POWER_PIN)
#define  PAM8406_POWER_L        GPIO_ResetBits(PAM8406_POWER_PORT,PAM8406_POWER_PIN)

void PAM8406_Init(void);
void PAM8406_Close(void);
void PAM8406_Open(void);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
