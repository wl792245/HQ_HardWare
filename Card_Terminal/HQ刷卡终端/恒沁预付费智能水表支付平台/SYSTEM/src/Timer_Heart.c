/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Timer_Heart.h
 * ��Ŀ���� ����ʱ�����ֽӿ����    
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-25
**********************************************************************************/
#include "Timer_Heart.h"
#include "stm32f10x_tim.h"

/*******************************************************************************
* �� �� ��         : TIM3_Heart_Init
* ��������		     : ��ʱ���ֳ�ʼ��
* ��    ��         : ʱ��=(arr+1)*(psc+1)/72M,m_irq����ռ���ȼ���1����s_irq����Ӧ���ȼ���0��
* ��    ��         : 
*******************************************************************************/
void TIM3_Heart_Init(u16 arr,u16 psc,u16 m_irq, u16 s_irq)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;	 
  NVIC_InitTypeDef NVIC_InitStructure;
	/* ������ʱ��3ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);			//���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_TimeBaseInitStructure.TIM_Period = arr;			//�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//����������ΪTIMxʱ��Ƶ��Ԥ��Ƶֵ��100Khz����Ƶ��
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;	 //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);	
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
	
		/* �����жϲ����������ж� */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );	//ʹ�ܻ���ʧ��ָ����TIM�ж�

	/* ����NVIC���� */
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 				//��TIM3_IRQn��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=m_irq;	//��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=s_irq;  			//��Ӧ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ��
	NVIC_Init(&NVIC_InitStructure);	 
}
/*******************************************************************************
* �� �� ��         : TIM2_Heart_Init
* ��������		     : ��ʱ���ֳ�ʼ��
* ��    ��         : ʱ��=(arr+1)*(psc+1)/72M,m_irq����ռ���ȼ���1����s_irq����Ӧ���ȼ���0��
* ��    ��         : 
*******************************************************************************/
void TIM2_Heart_Init(u16 arr,u16 psc,u16 m_irq, u16 s_irq)
{
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʹ��TIM2ʱ��
	//����жϱ�־λ
  TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//TIM_ClearFlag(TIM2,TIM_FLAG_Update);//����������ͬ
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);     //ʹ��ָ����TIM2�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);     //ʹ�ܶ�ʱ��2
}

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
