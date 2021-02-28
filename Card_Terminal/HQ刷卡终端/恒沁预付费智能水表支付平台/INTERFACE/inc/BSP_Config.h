/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��BSP_Config.h
 * ��Ŀ���� ������Ӳ���ӿڵ���       
 * ����ƽ̨ ������STM32F103C8T6оƬ����
 * Ӳ������ ��
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-01-25
**********************************************************************************/
#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#include "M6312.h"
#include "AT24C02.h"
#include "USB_Config.h"
#include "PC_Config.h"
#include "PAM8406_Config.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"
#include "HDMI_T050.h"
#include "Timer_Deal.h"
#include "sys.h"
#include "systick.h"
#include "CardInfoDeal.h"
#include "String_Config.h"

void BSP_Init(void);
void Remote_ChangeIp(void);
#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
