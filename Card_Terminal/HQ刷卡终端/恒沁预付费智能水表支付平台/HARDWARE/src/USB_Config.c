/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：USB_Config.c
 * 项目描述 ：开发板上USB接口设计       
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#include "USB_Config.h"
#include "systick.h"
#include "usb_lib.h"

#include "usb_pwr.h"	
#include "String_Config.h"
#include "string.h"
/*******************************************************************************
* 函 数 名         : USB_CONFIG_Init
* 函数功能		     : USB初始化   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USB_CONFIG_Init(u16 m_irq, u16 s_irq)
{
	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接
	Set_USBClock();
	USB_Interrupts_Config(m_irq, s_irq);    
 	USB_Init();	
}
/*******************************************************************************
* 函 数 名         : USB_PC_SendData
* 函数功能		     : 通过USB向电脑发送数据
* 输    入         : buf:字符串
* 输    出         : 无
*******************************************************************************/
void USB_PC_SendData(char * buf)
{
	String_Clear((char *)USB_USART_RX_BUF,strlen((const char *)USB_USART_RX_BUF));
	USB_USART_RX_STA=0;
	USB_Write(buf);	
	delay_ms(20);	
}









/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
