/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��sys.c
 * ��Ŀ���� ��ϵͳ���ó�ʼ�����       
 * ����ƽ̨ ������STM32F103C8T6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2020-10-29
**********************************************************************************/
#include "sys.h"
//#include "stm32f10x_pwr.h"
//********************************************************************************
void NVIC_Configuration(void)
{

	  //�ж����ȼ�����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : ϵͳʱ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
	
	//ʹ���ⲿ����
	RCC_HSEConfig(RCC_HSE_ON);
	
	//�ȴ��ⲿ�����ȶ�
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	//����ⲿ���������ɹ����������һ������
	if (HSEStartUpStatus==SUCCESS)
	{
		//����HCLK��AHBʱ�ӣ�=SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//PCLK1(APB1) = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//PCLK2(APB2) = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//FLASHʱ�����
		//�Ƽ�ֵ��SYSCLK = 0~24MHz   Latency=0
		//        SYSCLK = 24~48MHz  Latency=1
		//        SYSCLK = 48~72MHz  Latency=2
//		FLASH_SetLatency(FLASH_Latency_2);
//		//����FLASHԤȡָ����
//		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL���� SYSCLK/1 * 6 = 12*1*6 = 72MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		
		//����PLL
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLL�ȶ�
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//ϵͳʱ��SYSCLK����PLL���
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�
		while (RCC_GetSYSCLKSource()!=0x08);
	}
	return;
}
/*******************************************************************************
* �� �� ��         : PWR_EnterSLEEPMode
* ��������		     : ����˯��ģʽ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
//void PWR_EnterSLEEPMode(u32 SysCtrl_Set, u8 PWR_SLEEPEntry)//˯��ģʽ
//{
//    if (SysCtrl_Set)
//         SCB->SCR |= SysCtrl_SLEEPONEXIT_Set;   
//    else
//        SCB->SCR &= ~SysCtrl_SLEEPONEXIT_Set;
//	  SCB->SCR &= ~SysCtrl_SLEEPDEEP_Set;   
//    if(PWR_SLEEPEntry == PWR_SLEEPEntry_WFI)          
//        __WFI();                                       
//    else
//        __WFE();      
//}

//void INTX_ENABLE(void)
//{
//	__ASM volatile("cpsie i");		  
//}
/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************/
