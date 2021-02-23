/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��HXCard_Config.c
 * ��Ŀ���� ������ģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��HX(CS)--PA1
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#include "HXCard_Config.h"
#include "stm32f10x_exti.h"

volatile uint32_t   HXCardIsInter=0; //4428���жϱ�־λ
/*******************************************************************************
* �� �� ��         : HXCard_Init
* ��������		     : ����ģ���ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HXCard_Init(u16 m_irq, u16 s_irq)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
		//����gpio������ֹ�忨�ж�,PA1
		GPIO_InitStructure.GPIO_Pin=HXCARD_INIT_CSW;	 //ѡ����Ҫ���õ�IO��(pin_1)
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	  //������������ģʽ
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
		GPIO_Init(HXCARD_INIT_Port,&GPIO_InitStructure); /* ��ʼ��GPIO */	
		/*��GPIO�ܽ����ⲿ�ж�������*/
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);//ѡ��GPIO�ܽ������ⲿ�ж���·
		/*����EXTI*/
		EXTI_InitStructure.EXTI_Line=HXCARD_Line;
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//�½��ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure); 
		
		/* ����NVIC���� */	
		NVIC_InitStructure.NVIC_IRQChannel = HXCARD_INIT_IRQn;		//��EXTI1��ȫ���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //��ռ���ȼ�Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //��Ӧ���ȼ�Ϊ2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //ʹ��
		NVIC_Init(&NVIC_InitStructure);	
}








/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
