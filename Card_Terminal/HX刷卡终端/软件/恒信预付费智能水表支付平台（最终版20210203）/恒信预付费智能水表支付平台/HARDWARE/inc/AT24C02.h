/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��AT24C02.h
 * ��Ŀ���� ��24C02�ⲿ�ӿڵ���       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��24C02_SCL-PB10 24C02_SDA-PB11
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#ifndef __AT24C02_H_
#define __AT24C02_H_

#include "stm32f10x.h"


//�豸��Ϣ�ṹ�嶨��
typedef struct Terminal
{
	char TerminalId[20];			       //������
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //��Ļ�汾��
	char TerminalV[5];				       //�豸�汾��
	char QRcode[44];				         //��ά��
	char SignalStrength[3];			     //�ź�ǿ��
	char ReStartTime;                //��������
}TerminalInit;

//��������Ϣ�ṹ�嶨��
typedef struct Server
{
	char ServerIP[30];				       //������IP
	char ServerPort[10];			       //�������˿ں�
}ServerInit;

typedef struct Device
{
	ServerInit ServerInfo;
	TerminalInit TerminalInfo;
}DeviceInit, *pDeviceInit;

extern DeviceInit DeviceInfo;

void AT24C02_Init(void);
void AT24C02_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);
u8 AT24C02_Write(u16 WriteAddr,char *pBuffer,u16 NumToWrite);
void AT24C02IPTid_Init(void);
#endif 
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
