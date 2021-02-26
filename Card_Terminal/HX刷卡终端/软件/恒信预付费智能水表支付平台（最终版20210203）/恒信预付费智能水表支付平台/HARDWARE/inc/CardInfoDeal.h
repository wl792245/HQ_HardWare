/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��CardInfoDeal.h
 * ��Ŀ���� �����ݶ�ȡ�봦��42��.57��.24��
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __CARDINFODEAL_H_
#define __CARDINFODEAL_H_

#include "stm32f10x.h"
#include "IIC_Card.h"
#include "Card42_Config.h"
#include "Card24_Config.h"
#include "Card57_Config.h"
//24��.42����Դ��������
#define Card_POWER       GPIO_Pin_5 //24.42����Դ��
//24��.42���ж���������
#define Card_ICSW        GPIO_Pin_9 //24.42���жϽ�
#define Card_GPIO        GPIO_I2C 

#define Card_Line        EXTI_Line9
#define Card_IRQn        EXTI9_5_IRQn    //���ж�
#define Card_IRQHandler  EXTI9_5_IRQHandler //�ж�������

//��ʮ�����ư��ֽ�����
typedef struct CardHex
{
	u8 hexcid[15];          //����ʮ�����ư��ֽ�����
	u8 hexaid[15];          //������ʮ�����ư��ֽ�����
	u8 hexamount[15];       //��������ʮ�����ư��ֽ�����
  u8 hexbuyamount[15];    //��ֵ��ʮ�����ư��ֽ�����
	u8 hextotal[15];        //����ʮ������
	u8 hextime[15];         //����ʮ������
	u8 hexsecurity[15];     //����λʮ������
  u8 hexoneprice[15];     //һ��ʮ������
	u8 hextwoprice[15];     //����ʮ������
	u8 hexthreeprice[15];   //����ʮ������
	u8 hexfourprice[15];    //�ļ�ʮ������
	u8 hexoneamount[15];    //һ��ʮ������
	u8 hextwoamount[15];    //����ʮ������
	u8 hexthreeamount[15];  //����ʮ������
}Card_Hex;

//��ʮ������ת�ַ�����
typedef struct CardChar
{
	u8 strcid[15];          //����ʮ������ת�ַ�����
	u8 straid[15];          //������ʮ������ת�ַ�����
	u8 stramount[15];       //��������ʮ������ת�ַ�����
  u8 strbuyamount[15];    //��ֵ��ʮ������ת�ַ�����
	u8 strtotal[15];        //�����ַ�
	u8 strtimes[15];        //��ֵ�����ַ�
	u8 strsecurity[15];     //����λ�ַ�
	u8 stroneprice[15];     //һ���ַ�
	u8 strtwoprice[15];     //�����ַ�
	u8 strthreeprice[15];   //�����ַ�
	u8 strfourprice[15];    //�ļ��ַ�
	u8 stroneamount[15];    //һ���ַ�
	u8 strtwoamount[15];    //�����ַ�
	u8 strthreeamount[15];  //�����ַ�
}Card_Char;

//����������Ϣ�ṹ��
typedef struct CardDeal
{
	char TSendCID[100];          //�ϴ������������ַ���
	u8 *Card_Head;              //��ͷ�ַ�
	u8 Card_Flag;               //��ˢ���ɹ���־λ 0ʧ�� 1�ɹ�
	Card_Hex Card_HexInfo;      //��ʮ�����ư��ֽ�����
  Card_Char Card_CharInfo;    //��ʮ������ת�ַ�����
}Card_DealSend;

//�����������Ϣ�ṹ��
typedef struct CardDealRecv
{
	char head[15];             //�����������ַ�����ͷ �����ж���ʲô���͵Ŀ�
	char terminalId[20];        //������
	char cardId[15];           //����
	char orderId[40];          //������
	char userId[40];           //�û���
	char useradress[40];       //�û���ַ
	char userproperties[40];   //��ˮ����
	char Totalamount[40];      //�û�ˮ����ˮ������
	char LastAllBuyMount[40];  //�û����깺ˮ��
	char Buyamout[40];         //��ֵ��
	char total[15];             //����
	char pricemountstr[30];    //��ֵ���ַ���
	char times[15];             //����
	char security[15];          //����λ
	char oneprice[15];          //һ��
	char twoprice[15];          //����
	char threeprice[15];        //����
	char fourprice[15];         //�ļ�
	char oneamount[15];         //һ��
	char twoamount[15];         //����
	char threeamount[15];       //����
}Card_DealRecv;

extern volatile uint32_t   CardIsInter;

void Card_Init(u16 m_irq, u16 s_irq);
u8 CopyCIDTo24C02(void);
u8 CopyScrrenvTo24C02(void);
u8 CopyTerminalVTo24C02(void);
u8 CopyIPPortTo24C02(void);
u8 CopyQRcodeTo24C02(void);

void Card24_PDeal(Card24_PreInfo *Info24p);
void Card24_LDeal(Card24_LadderInfo *Info24l);
void Card42_PDeal(Card42_PreInfo *Info42p);
void Card57_PDeal(Card57_PreInfo * Info57p);
void Card57_LDeal(Card57_LadderInfo *Card57l);
void CacheOrder_update(void);

#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
