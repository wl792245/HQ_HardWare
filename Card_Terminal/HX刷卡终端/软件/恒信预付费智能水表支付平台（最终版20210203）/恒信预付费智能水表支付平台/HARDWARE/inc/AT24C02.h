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

#define FISRT "HX"
#define FTerminalIDServerIP "1912W200xx%89860411101850078642%V3.1%V1.9%121.196.181.103%03306%xh2xqWfEYjodrSPK90jU%"

//�豸��Ϣ�ṹ�嶨��
typedef struct Terminal
{
	char TerminalId[15];			       //������
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //��Ļ�汾��
	char TerminalV[5];				       //�豸�汾��
	char QRcode[26];				         //��ά��
	char SignalStrength[3];			     //�ź�ǿ��
	char ReStart[5];                 //������������
}TerminalInit;

//��������Ϣ�ṹ�嶨��
typedef struct Server
{
	char ServerIP[18];				       //������IP
	char ServerPort[6];			       //�������˿ں�
}ServerInit;

typedef struct Device
{
	ServerInit ServerInfo;
	TerminalInit TerminalInfo;
}DeviceInit, *pDeviceInit;

extern DeviceInit DeviceInfo;
extern volatile int Restart_Count;

void AT24C02_Init(void);
void AT24C02_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead);
u8 AT24C02_Write(u16 WriteAddr,char *pBuffer,u16 NumToWrite);
void AT24C02IPTid_Init(void);
void ResStart_Write(char * ReStartInfo, int value);
int ReStart_Read(char *ReStartInfo);
#endif 
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
