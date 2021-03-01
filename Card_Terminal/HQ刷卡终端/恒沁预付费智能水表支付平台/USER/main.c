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

/*******************************************************************************
* 函 数 名         : main
* 函数功能		     : 主函数，程序入口
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
int main(void)
{
  u8 i;
	BSP_Init();
	while(1)
	{
		Remote_ChangeIp();  //远程IP二维码更改
		//三川卡处理
		if(SCCardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			TIM_Cmd(TIM2,DISABLE); //使能或者失能TIMx外设
			Server_Time = 0;
			PC_Time = 0;
			SCCardIsInter = 0;
			HDMIShowWait();
			SCCard_Deal();
		  while(GPIO_ReadInputDataBit(SCCARD_INIT_Port,SCCARD_INIT_CSW)==0)	//卡物理在线，等待
			{
				;
			}
			HDMIShowMenuInfo(&DeviceInfo);        //主界面显示
			TIM_Cmd(TIM2,DISABLE); //使能或者失能TIMx外设
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
		}
		//定时器服务器处理
		if(Server_TimeIsInter == 1)
		{
			Server_TimeIsInter = 0;
			
		}
		//定时器电脑处理
		if(PC_TimeIsInter == 1)
		{
			PC_TimeIsInter = 0;
		}
		//电脑重启处理
		if (PC_RestartFlag == 0x86)
		{
			TIM_Cmd(TIM2,DISABLE); //使能或者失能TIMx外设
			PC_Time = 0;
			PC_RestartFlag = 0;
			PC_Close();
			delay_ms(941);
      PC_Open();                         //微电脑开机
			for(i=0;i<180;i++)
			{
				delay_ms(941);
			}
			if(PC_Start())
			{
				HDMIShowMenuInfo(&DeviceInfo);     //主界面显示
				TIM_Cmd(TIM2,ENABLE); //使能或者失能TIMx外设
			}
		}
		//M6312重启
		if (M6312_RestartFlag == 0x63)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			Server_Time = 0;
			M6312_RestartFlag = 0;
			M6312_Connect(&DeviceInfo, "网络重连中...");  //M6312连接服务器,信号强度检测，时间读取
			FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId); //第一次握手
			HDMIShowMenuInfo(&DeviceInfo);        //主界面显示
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设,服务器定时器重新开始计时
		}
	}
}





/******************* (C) COPYRIGHT 2021 江苏恒沁科技有限公司 *****END OF FILE****/
