/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：main.c
 * 项目描述 ：恒信刷卡终端设备    
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 版  本   ：V2.0
 * 时  间   ：2020-12-31
**********************************************************************************/
#include "BSP_Config.h"  



Card24_LadderInfo   Info24ladder;      //24卡阶梯信息结构体
Card24_PreInfo      Info24pre;         //24卡预付费信息结构体
Card42_PreInfo      Info42pre;         //42卡预付费信息结构体
Card57_PreInfo      Info57pre;
Card57_LadderInfo   Info57ladder;	

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
		//24.42卡处理
		if(CardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			sec = 0;
			CardIsInter = 0;
			Card24_LadderReadCID(&Info24ladder);   	//读24阶梯卡
			Card42_PreReadCID(&Info42pre);			    //读4442预付费卡
			HDMIShowWait();                         //液晶显示“请稍后...x%”
			if(Info42pre.cflag == 0xB2)             //4442预付费卡
			{
					Card42_PDeal(&Info42pre);		
			}
			else if(Info24ladder.cflag == 0xC1)      //设备号修改卡
			{
					if(CopyCIDTo24C02())                  //将配置卡的机器号IICID复制到开发板上的24c02
					{
						XFS_HDMI_Play("设备号更改失败，请重试");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("设备号更改成功");	
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag == 0xC2)     //屏幕版本号修改卡
			{
					if(CopyScrrenvTo24C02())                 //将配置卡的屏幕版本号复制到开发板上的24c02
					{
						XFS_HDMI_Play("屏幕版本号更改失败，请重试");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("屏幕版本号更改成功");
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag == 0xC3)     //设备程序版本号修改卡
			{
					if (CopyTerminalVTo24C02())               //将配置卡的设备版本号复制到开发板上的24c02
					{
						XFS_HDMI_Play("设备版本号更改失败，请重试");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("设备版本号更改成功");
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag==0xC5)      //显示卡
			{
					HDMIShowServerInfo(&DeviceInfo);
					delay_ms(941);
			}
			else if ( Info24ladder.areacode[0]==0&&Info24ladder.areacode[1]==0&&Info24ladder.areacode[2]==0) // 24预付费卡
			{
					Card24_PreReadCID(&Info24pre);       //先读24卡
					if ((Info24pre.watersalesamount[0]==0)&&(Info24pre.watersalesamount[1]==0)) //24预付费卡售水位为0才可以刷卡，不为零则先插水表
					{
						Card24_PDeal(&Info24pre);
					}
					else
					{
						XFS_HDMI_Play("卡余量不为零，先插表");
					}
					
			}
			else if ((Info24ladder.areacode[0]!=0||Info24ladder.areacode[1]!=0||
					Info24ladder.areacode[2]!=0)&&(Info24ladder.areacode[0]!=0xFF||
					Info24ladder.areacode[1]!=0xFF||Info24ladder.areacode[2]!=0xFF))//24阶梯卡
			{
				Card24_LDeal(&Info24ladder);
			}
			else
			{
				XFS_HDMI_Play("请重刷");
			}
			while(GPIO_ReadInputDataBit(Card_GPIO,Card_ICSW)==0)	//卡物理在线，等待
			{
				;
			}
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
			HDMIShowMenuInfo(&DeviceInfo);
		}
		//57卡处理
		if (Card57_Flag == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			Card57_Flag = 0;
			sec = 0;
			Card57_PreReadCID(&Info57pre);        //读57预付费卡
			HDMIShowWait();                         //液晶显示“请稍后...x%”
			if (Info57pre.cflag == 0xB4)           //5557卡为预付费卡的处理
			{
				Card57_PDeal(&Info57pre);
			}
			else if (Info57pre.cflag == 0xB6)     ////阶梯表处理
			{
				Card57_LadderReadCID(&Info57ladder);       //读57阶梯卡
				Card57_LDeal(&Info57ladder);
			}
			else
			{
				XFS_HDMI_Play("请重刷");
			}
		  while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_ICSW)==0)	//卡物理在线，等待
			{
				;
			}
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
			HDMIShowMenuInfo(&DeviceInfo);
		}	
    //定时器处理
		if(TimeIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			TimeIsInter = 0;
			TIM_Heart_Deal();
			HDMIShowMenuInfo(&DeviceInfo);
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
		}
		//重启M6312
    if (M6311_RestartFlag == 0x63)
		{
			TIM_Cmd(TIM3,DISABLE); //使能或者失能TIMx外设
			M6311_RestartFlag = 0;
			sec = 0;
			M6311_Connect(&DeviceInfo, "网络重连中...");  //M6311连接服务器
			FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId);
			HDMIShowMenuInfo(&DeviceInfo);
			TIM_Cmd(TIM3,ENABLE); //使能或者失能TIMx外设
		}
	}
}

/******************* (C) COPYRIGHT 2020 江苏恒沁科技有限公司 *****END OF FILE****/
