/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��HDMI_T050.h
 * ��Ŀ���� ��Һ����T050�봮��5ͨѶ     
 * ����ƽ̨ ������STM32F103VBT6оƬ����
 * Ӳ������ ��HDMI_RX-PB10(TX)   HDMI_TX-PB11(RX)
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-8
**********************************************************************************/
#ifndef __HDMI_T050_H_
#define __HDMI_T050_H_

#include "stm32f10x.h"
#include "AT24C02.h"
#include "Usart3_HDMI.h"

#define  USART_HDMI_End  USART_HDMI_SendB(0xff,3)  //������

/*******************************************************************************
 userId���û�ID
 userAdress���û���ַ
 waterProperties����ˮ����
 totalAmount���ۼƹ�������
 LastAllBuyMount�����깺������
 Unit:��λ ���� Unit=1  ��ʾ Ԫ��  Ԥ����   Unit=0  ��ʾ  ��	
 islongthan12���ַ����ȳ���12������ʾ
*******************************************************************************/
//�û���Ϣ
typedef struct UserType
{
	char *userId;
	char *userAdress;
}User;
//��ˮ��Ϣ
typedef struct WaterType
{
	char *waterProperties;
	char *totalAmount;
	char *LastAllBuyMount;
	int Unit;
}Water;
//page4����ʾ
typedef struct Show
{
	User UserInfo;
	Water WaterInfo;
	int islongthan12;
}UserInfoShow, *PUserInfoShow;

void HDMI_Init(u32 Bound);
void HDMI_CMD_Send(char *cmd);

void NetSignalDisplay(u8 SignalValue);
void HDMISendConncetOK(void);
void HDMIShowRechargeSucess(char *RechargeSucessValue,int Unit);
void HDMIShowUserDetail(UserInfoShow UserInfoDisplay);
void HDMIShowInfo(char *reason);
void HDMIShowWait(void);
void HDMIShowWaitEnd(void);
void HDMIShowChangeSuccess(void);
void HDMIShowSignal(unsigned char SignalValue);
void HDMIShowServerInfo(pDeviceInit deviceinfo);
void HardWareErr(void);
void HDMIShowMenuInfo(DeviceInit Menu);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
