/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Card57_Config.h
 * ��Ŀ���� ��57���ӿڵ���       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��Card57_CFE--PC0  Card57_OUT--PC1	Card57_POWER--PC3 Card57_ICSW--PC2
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#ifndef __CARD57_CONFIG_H_
#define __CARD57_CONFIG_H_
#include "stm32f10x.h"
#include "IIC_Card.h"

#define	CARD57_MAX      150


#define Card57_CFE       GPIO_Pin_0 
#define Card57_OUT       GPIO_Pin_1 
#define Card57_POWER     GPIO_Pin_3 
#define Card57_ICSW      GPIO_Pin_2 
#define Card57_GPIO      GPIOC 

#define  Card57_OUTPUT_H GPIO_SetBits(Card57_GPIO,Card57_OUT)
#define  Card57_OUTPUT_L GPIO_ResetBits(Card57_GPIO,Card57_OUT)
#define  Card57_CFE_H    GPIO_SetBits(Card57_GPIO,Card57_CFE)
#define  Card57_CFE_L    GPIO_ResetBits(Card57_GPIO,Card57_CFE)
#define  Card57_POWER_H  GPIO_SetBits(Card57_GPIO,Card57_POWER)
#define  Card57_POWER_L  GPIO_ResetBits(Card57_GPIO,Card57_POWER)

#define Card57_Line        EXTI_Line2
#define Card57_IRQn        EXTI2_IRQn    //���ж�
#define Card57_IRQHandler  EXTI2_IRQHandler //�ж�������

//57��Ԥ������Ϣ�ṹ��
typedef struct C57prepaymentInfo	
{
		u8 cardId[5];//����
		u8 cflag;//����־λ
		u8 watersalesamount[2];//��ǰ��ˮ�� Ԫ
		u8 total[2]; //�ۼ����۶�	
}Card57_PreInfo;

//57��������Ϣ�ṹ��
typedef struct C57LadderpaymentInfo	
{
		u8 cardId[5];//����
		u8 cflag;//����־λ
		u8 watersalesamount[4];//��ˮ��֣�		
		u8 ladderprice[6];//��		
		u8 ladderamount[3];//��
		u8 total[3]; 	
}Card57_LadderInfo;





extern volatile uint32_t   Card57_Flag;

void Card57_Init(u16 m_irq, u16 s_irq);

void Card57_PreReadCID(Card57_PreInfo *Card57Pre);
u8 Card57_PreWrite(Card57_PreInfo Card57Pre);

void Card57_LadderReadCID(Card57_LadderInfo *Card57Ladder);
u8 Card57_LadderWriteCID(Card57_LadderInfo Card57Ladder);
#endif
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
