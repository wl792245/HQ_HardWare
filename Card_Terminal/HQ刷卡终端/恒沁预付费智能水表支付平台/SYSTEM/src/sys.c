#include "sys.h"


//********************************************************************************  
void NVIC_Configuration(void)
{

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
  //RCC_ClocksTypeDef get_rcc_clock;
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();			                 //��λʱ��
	SystemInit();	                     //ϵͳʱ�ӳ�ʼ��
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
		//FLASH_SetLatency(FLASH_Latency_2);
//		//����FLASHԤȡָ����
		//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL���� SYSCLK/1 * 6 = 12*1*6 = 72MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		
		//����PLL
		RCC_PLLCmd(ENABLE);
		//�ȴ�PLL�ȶ�
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//ϵͳʱ��SYSCLK����PLL���
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//RCC_GetClocksFreq(&get_rcc_clock);
		//�л�ʱ�Ӻ�ȴ�ϵͳʱ���ȶ�
		while (RCC_GetSYSCLKSource()!=0x08);

	}

	return;
}
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

