/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��AT24C02.h
 * ��Ŀ���� ��24C02�ⲿ�ӿڵ���       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��24C02_SCL-PB2 24C02_SDA-PB0
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#ifndef __AT24C02_H_
#define __AT24C02_H_

#include "stm32f10x.h"

#define FISRT "Hx"
#define FTerminalIDServerIP "1912W200xx%89860411101850078642%V3.1%V1.9%121.196.181.103%03306%xh2xqWfEYjodrSPK90jU%"      


//ѡ������
typedef enum CardType
{
	HX4442 = 0,  //����4442��
	HX4428,      //����4428��
	XT5557,      //����5557��
	SC5557,      //����5557��
//	YZ4424,      //����4424��
//	YZ4442,      //����4442��
//	YZ5557,      //����5557��
}CardType_Info;

//�豸����
typedef struct Data
{
	char Year[3];			               //��
	char Mounth[3];			             //��
	char Day[3];				             //��
	char Hour[3];			               //ʱ
	char Minute[3];			             //��
}DataInit, *PDataInit;
//�豸��Ϣ�ṹ�嶨��
typedef struct Terminal
{
	char TerminalId[15];			       //������
	char ICCID[25];					         //ICCID
	char ScreenV[5];				         //��Ļ�汾��
	char TerminalV[5];				       //�豸�汾��
	char QRcode[26];				         //��ά��
	char SignalStrength[3];			     //�ź�ǿ��
	char Fault;			                 //��������
}TerminalInit;

//��������Ϣ�ṹ�嶨��
typedef struct Server
{
	char ServerIP[18];				       //������IP
	char ServerPort[6];			       //�������˿ں�
}ServerInit;

typedef struct Device
{
	DataInit   DataInfo;
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
