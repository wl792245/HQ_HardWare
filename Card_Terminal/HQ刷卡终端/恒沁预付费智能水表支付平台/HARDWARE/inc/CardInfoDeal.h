/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��CardInfoDeal.h
 * ��Ŀ���� �����ݶ�ȡ�봦������4428��4442
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-2-2
**********************************************************************************/
#ifndef __CARDINFODEAL_H_
#define __CARDINFODEAL_H_

#include "stm32f10x.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"

typedef struct CARDPC
{
	char Card_PCRecHead[2];                 //PC����DLLͷ
  char Card_PCRecStr[200];                //PC����DLL����
	char Card_iCardType[4];               //����������
	char Card_iManageCode[9];             //�����Ӽ�ά����
	char Card_iKey[15];                   //�����Ӽ���Կ
	char Card_CardId[9];                  //��������
	char Card_BiaoId[9];                  //�������
	char Card_BuyNum[3];                  //������ֵ���
	char Card_CardMoneny[8];              //�������Ͻ��֣�
	char Card_BuyAllNum[4];               //������ֵ�����
	char Card_UseWater[7];                //������ˮ��0.1��
	char Card_ZhuangTaiMa[9];             //����״̬��
}Card_PC, *PCard_PC;

typedef struct CARDSERVER
{
	char Card_ServerRecHead[18];        //������ȡͷ
  PSCCard_Server SCCard_Info;         //��������Ϣ
}Card_Server, *PCard_Server;

void SCCard_Deal(void);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
