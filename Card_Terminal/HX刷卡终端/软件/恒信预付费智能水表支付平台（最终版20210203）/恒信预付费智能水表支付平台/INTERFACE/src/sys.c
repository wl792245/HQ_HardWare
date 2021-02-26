/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：sys.c
 * 项目描述 ：系统配置初始化设计       
 * 开发平台 ：基于STM32F103C8T6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2020-10-29
**********************************************************************************/
#include "sys.h"
//#include "stm32f10x_pwr.h"
//********************************************************************************
void NVIC_Configuration(void)
{

	  //中断优先级设置
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
	ErrorStatus HSEStartUpStatus;
	
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
//		FLASH_SetLatency(FLASH_Latency_2);
//		//开启FLASH预取指功能
//		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		//PLL设置 SYSCLK/1 * 6 = 12*1*6 = 72MHz
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		
		//启动PLL
		RCC_PLLCmd(ENABLE);
		//等待PLL稳定
		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

		//系统时钟SYSCLK来自PLL输出
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		
		//切换时钟后等待系统时钟稳定
		while (RCC_GetSYSCLKSource()!=0x08);
	}
	return;
}
/*******************************************************************************
* 函 数 名         : PWR_EnterSLEEPMode
* 函数功能		     : 进入睡眠模式
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
//void PWR_EnterSLEEPMode(u32 SysCtrl_Set, u8 PWR_SLEEPEntry)//睡眠模式
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
/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************/
