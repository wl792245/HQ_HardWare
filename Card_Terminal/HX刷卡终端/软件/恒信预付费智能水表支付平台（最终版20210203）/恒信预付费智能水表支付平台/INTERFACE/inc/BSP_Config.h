/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��BSP_Config.h
 * ��Ŀ���� ������Ӳ���ӿڵ���       
 * ����ƽ̨ ������STM32F103C8T6оƬ����
 * Ӳ������ ��
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2020-10-29
**********************************************************************************/
#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

//**************�Դ���ͷ�ļ�********************//
#include "sys.h"
#include "stm32f10x_tim.h"
//**************�Ա��ͷ�ļ�********************//
#include "CardInfoDeal.h"
#include "AT24C02.h"
#include "HDMI_T050.h"
#include "M6311.h"
#include "Timer_Deal.h"
#include "XFS.h"
#include "systick.h"
#include "sys.h"
#include "string.h"
#include "IWDG.h"
#include "Timer_Heart.h"
#include "String_Get.h"
//**************�ⲿ������������****************//

//**************��д��������********************//
void BSP_Init(void);

#endif

/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************/
