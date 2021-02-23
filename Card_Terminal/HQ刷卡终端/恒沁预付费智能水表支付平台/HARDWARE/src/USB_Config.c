/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��USB_Config.c
 * ��Ŀ���� ����������USB�ӿ����       
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#include "USB_Config.h"
#include "systick.h"
#include "usb_lib.h"

#include "usb_pwr.h"	
#include "String_Config.h"
#include "string.h"
/*******************************************************************************
* �� �� ��         : USB_CONFIG_Init
* ��������		     : USB��ʼ��   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void USB_CONFIG_Init(u16 m_irq, u16 s_irq)
{
	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
	USB_Port_Set(1);	//USB�ٴ�����
	Set_USBClock();
	USB_Interrupts_Config(m_irq, s_irq);    
 	USB_Init();	
}
/*******************************************************************************
* �� �� ��         : USB_PC_SendData
* ��������		     : ͨ��USB����Է�������
* ��    ��         : buf:�ַ���
* ��    ��         : ��
*******************************************************************************/
void USB_PC_SendData(char * buf)
{
	String_Clear((char *)USB_USART_RX_BUF,strlen((const char *)USB_USART_RX_BUF));
	USB_USART_RX_STA=0;
	USB_Write(buf);	
	delay_ms(20);	
}









/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
