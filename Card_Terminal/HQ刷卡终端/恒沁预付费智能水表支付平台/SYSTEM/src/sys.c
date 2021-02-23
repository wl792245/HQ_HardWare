#include "sys.h"


//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : 系统时钟设置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  //RCC_ClocksTypeDef get_rcc_clock;
	ErrorStatus HSEStartUpStatus;
	RCC_DeInit();			                 //复位时钟
	SystemInit();	                     //系统时钟初始化
	//使能外部晶振
	RCC_HSEConfig(RCC_HSE_ON);
	
	//等待外部晶振稳定
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	//如果外部晶振启动成功，则进行下一步操作
	if (HSEStartUpStatus==SUCCESS)
	{
		//设置HCLK（AHB时钟）=SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);

		//PCLK1(APB1) = HCLK/2
		RCC_PCLK1Config(RCC_HCLK_Div2);

		//PCLK2(APB2) = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);

		//FLASH时序控制
		//推荐值：SYSCLK = 0~24MHz   Latency=0
		//        SYSCLK = 24~48MHz  Latency=1
		//        SYSCLK = 48~72MHz  Latency=2
		//FLASH_SetLatency(FLASH_Latency_2);
//		//开启FLASH预取指功能
		//FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL设置 SYSCLK/1 * 6 = 12*1*6 = 72MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		
		//启动PLL
		RCC_PLLCmd(ENABLE);
		//等待PLL稳定
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//系统时钟SYSCLK来自PLL输出
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		//RCC_GetClocksFreq(&get_rcc_clock);
		//切换时钟后等待系统时钟稳定
		while (RCC_GetSYSCLKSource()!=0x08);

	}

	return;
}
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

