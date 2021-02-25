/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��SCCard_Config.h
 * ��Ŀ���� ��������ģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��SC(CS)--PB5
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-2-3
**********************************************************************************/
#ifndef __SCCARD_CONFIG_H_
#define __SCCARD_CONFIG_H_

#include "stm32f10x.h"


#define SCCARD_INIT_Port          GPIOB
#define SCCARD_INIT_CSW           GPIO_Pin_5
#define SCCARD_Line               EXTI_Line5
#define SCCARD_INIT_IRQn          EXTI9_5_IRQn
#define SCCARD_INIT_IRQHandler    EXTI9_5_IRQHandler

extern volatile uint32_t   SCCardIsInter;

typedef struct SCCARD5557PC
{
	char PCRecHead[2];                 //PC����DLLͷ
  char PCRecStr[200];                //PC����DLL����
	char iCardTypeSC[4];               //����������
	char iManageCodeSC[9];             //�����Ӽ�ά����
	char iKeySC[15];                   //�����Ӽ���Կ
	char CardIdSC[9];                  //��������
	char BiaoIdSC[9];                  //�������
	char BuyNumSC[3];                  //������ֵ���
	char CardMonenySC[8];              //�������Ͻ��֣�
	char BuyAllNumSC[4];               //������ֵ�����
	char UseWaterSC[7];                //������ˮ��0.1��
	char ZhuangTaiMaSC[9];             //����״̬��
}SCCard_5557PC, *PSCCard_5557PC;

typedef struct SCCARD5557SERVER
{
	char ServerRecHead[18];             //������ȡͷ
	char SCID[11];                     	//������
	char SC5557CardID[17];             	//����5557����
	char SC5557OrderId[60];            	//����������
	char iPrice1SC[10];									//�����۸�1 0.1��
	char iPrice2SC[10];									//�����۸�2 0.1��
	char iPrice3SC[10];									//�����۸�3 0.1��
	char iTon1SC[5];                    //��������1 ����ֻ�ж��׼���0-9999�����׼���0-99
	char iTon2SC[5];                    //��������2 0-99
	char BuyMonenySC[10];               //������ֵ���
	char SC5557UserID[17];              //����5557�û����
	char SCAdress[60];                  //�����û�i��ַ
	char SCXingzhi[30];                 //������ˮ����
	char SCTotalamount[10];             //����ˮ������
	char SCLastAllBuyMount[10];         //����������ˮ����
}SCCard_Server, *PSCCard_Server;

void SCCard_Init(u16 m_irq, u16 s_irq);
u8 SCCard_PCDataSpit(PSCCard_5557PC SCCardPC,char *pdst_ServerSend,char *psrc_PCRecv);
void SCCard_ServerDataSpit(PSCCard_Server SCCardServer,const SCCard_5557PC SCCardPC, char *pdst_ServerSend, char *pdst_PCRec, char *psrc_Server, char * PCSend);
void SCCard_ServerDataCombine(char *pdst_ServerSend, const SCCard_Server SCCardServer, char * WriteCardSuccess, int isSuccess);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
