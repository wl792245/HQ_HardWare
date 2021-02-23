/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：main.c
 * 项目描述 ：恒沁华旭刷卡终端设备    
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 版  本   ：V2.0
 * 时  间   ：2021-01-25
**********************************************************************************/
#include "BSP_Config.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "CardInfoDeal.h"
/*******************************************************************************
* 函 数 名         : main
* 函数功能		     : 主函数，程序入口
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
int main(void)
{

	BSP_Init();
	while(1)
	{
		//华旭卡处理
		if(HXCardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设	
			sec = 0;
			HXCardIsInter = 0;
			HDMIShowWait();
			
		}
		//三川卡处理
		if(SCCardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			sec = 0;
			SCCardIsInter = 0;
			HDMIShowWait();
			SCCard_Deal();
		}
		//新天卡处理
		//海通卡处理
		//扬州卡处理
		//定时器处理
		if(TimeIsInter == 1)
		{
			
		}
	}
}





/******************* (C) COPYRIGHT 2021 江苏恒沁科技有限公司 *****END OF FILE****/
