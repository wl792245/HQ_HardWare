/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��PC_Config.c
 * ��Ŀ���� ��PCģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#include "PC_Config.h"
#include "USB_Config.h"
#include "systick.h"
#include "string.h"
#include "String_Config.h"
#include "HDMI_T050.h"
#include "PAM8406_Config.h"

volatile unsigned char PC_Connecting = 1;   //΢��������������
volatile unsigned char PC_RestartFlag = 0;   //΢����������־λ
char SendBuf[220];  //���ͻ�����
/*******************************************************************************
* �� �� ��         : PCPOWER_Init
* ��������		     : PC��Դ���ų�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void PCPOWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//����΢������ϵͳ��ʼ��Ƕ��ʽ����ӿڡ�PLL������ϵͳcoclock������	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  //���û���ø���apb��apb2����Χʱ�ӡ�

	GPIO_InitStructure.GPIO_Pin=PC_POWER_PIN ;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;	  //��������50MHz
	GPIO_Init(PC_POWER_PORT,&GPIO_InitStructure); //����gpio_init�ṹ�е�ָ��������ʼ��gpiox��Χ��
}
/*******************************************************************************
* �� �� ��         : PC_Init
* ��������		     : PCģ���ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	PCPOWER_Init();
	USART_PC_Init(Bound, m_irq, s_irq);
	USB_CONFIG_Init(m_irq,s_irq);
}
/*******************************************************************************
* �� �� ��         : PC_Close
* ��������		     : ΢���Թر�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_Close(void)
{
	PC_POWER_L;
	delay_ms(1000);
}
/*******************************************************************************
* �� �� ��         : PC_Open
* ��������		     : ΢���Դ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_Open(void)
{
	PC_POWER_H;
	delay_ms(1000);
}
/*******************************************************************************
* �� �� ��         : USART_PC_SendData
* ��������		     : ͨ����������Է�������
* ��    ��         : buf:�ַ�����len:����
* ��    ��         : ��
*******************************************************************************/
void USART_PC_SendData(char * buf,uint32_t len)
{
	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len=0;
	USART_PC_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* �� �� ��         : PC_ReadCard
* ��������		     : ��Ƭ������Է��Ͷ�������������ȡ4442��4428�ԵĿ���Ϣ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_ReadCard(void)
{
	PC_SendData("ReadCard%$$");
}
/*******************************************************************************
* �� �� ��         : PC_ReadData
* ��������		     : ͨ�����ڻ���USB��PC��ȡ����
* ��    ��         : pdest:�����ַ���  times������ len:���ճ���
* ��    ��         : 1:�ɹ����յ����� 0����������ʧ��
*******************************************************************************/
u8 PC_ReadData(char * pdest,  int times,int len)
{
		u8 Sendtimes = 0;
	  u8 PC_ReturnValue = 0; 
	  if(Sendtimes < times) //û���ؾ���������3��
		{
			PC_ReadCard();         //�Ϸ�����ָ�����
			Sendtimes++;  
			PC_ReturnValue = PC_ReceiveData(pdest, len);
			if (1 == PC_ReturnValue)
			{
				return 1;
			}
		}
		return 0;
}
/*******************************************************************************
* �� �� ��         : PC_WriteCard
* ��������		     : ��Ƭ������Է���д��������
* ��    ��         : psrc:������Ϣ��CardType:�����ͣ�0-42����1-28����2-57��
* ��    ��         : ��
*******************************************************************************/
void PC_WriteCard(const char *psrc, CardType_Info CardTypeInfo)
{
	String_Clear(SendBuf, strlen(SendBuf));
	switch(CardTypeInfo)
	{
		case HX4442:
			strcpy(SendBuf,"Write4442Card%");
			break;
		case HX4428:
			strcpy(SendBuf,"Write4428Card%");
			break;
		case XT5557:
			strcpy(SendBuf,"Writee57Card%");
			break;
		case SC5557:
			strcpy(SendBuf,"WriteSC5557Card%");
			break;
	}
	strcat(SendBuf,psrc);		//�����ַ����ϲ�
	strcat(SendBuf,"%$$");
	PC_SendData(SendBuf);

}
/*******************************************************************************
* �� �� ��         : PC_WirteData
* ��������		     : ͨ�����ڻ���USB�ڶ�PC����д������ָ��
* ��    ��         : pdest:�����ַ�����psrc:д���ַ���  times������ len:���ճ��� CardTypeInfo:������
* ��    ��         : 1:�ɹ����յ����� 0����������ʧ��
*******************************************************************************/
u8 PC_WirteData(char * pdest, const char *psrc, int times,int len, CardType_Info CardTypeInfo)
{
	u8 Sendtimes = 0;
	u8 PC_ReturnValue = 0; 
	if (Sendtimes < times)
	{
		//����Է���д������
		PC_WriteCard(psrc, CardTypeInfo);
		Sendtimes++; 
		PC_ReturnValue = PC_ReceiveData(pdest, len);
		if (PC_ReturnValue == 1)
		{
			return 1;
		}
	}
	else
	{
		
	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : PC_ShutDown
* ��������		     : ��Ƭ������Է��͹ػ�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_ShutDown(void)
{
	PC_SendData("ShutDown%$$");
}
/*******************************************************************************
* �� �� ��         : PC_SendData
* ��������		     : ��Ƭ������Է���ָ��
* ��    ��         : data������ָ��
* ��    ��         : ��
*******************************************************************************/
void PC_SendData(char * data)
{
	USB_PC_SendData(data);
  USART_PC_SendData(data,strlen(data));
}
/*******************************************************************************
* �� �� ��         : PC_ReceiveData
* ��������		     : ��Ƭ�����յ��Դ��ص�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
u8 PC_ReceiveData(char * pdest, int len)
{
	u8 i;
	for (i=0; i<20; i++)  //�ȴ���������
	{
		if (usart2_rcv_len > 0) 
		{
			delay_ms(341);
			if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"%$$"))
			{
				String_Clear(pdest, len);
				PC_Usart_GetRcvData(pdest, usart2_rcv_len);
				break;
			}				
		}
		else if (USB_USART_RX_STA > 0) 
		{
			delay_ms(341);
			if (NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"%$$"))
			{
				String_Clear(pdest, len);
				PC_USB_GetRcvData(pdest, USB_USART_RX_STA);
				break;
			}				
		}
		delay_ms(600);
	}
	if (i < 20)
	{
		return 1;
	}
	else
	{
		return 0;
	}

}
/*******************************************************************************
* �� �� ��         : PC_Usart_GetRcvData
* ��������		     : ��Ƭ��ͨ�����ڽ��յ����Ե�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_Usart_GetRcvData(char * buf, int rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = usart2_rcv_buf[i];
	}
	buf[i] = '\0';
	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len = 0;
}
/*******************************************************************************
* �� �� ��         : PC_USB_GetRcvData
* ��������		     : ��Ƭ��ͨ�����ڽ��յ����Ե�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_USB_GetRcvData(char * buf, int rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = USB_USART_RX_BUF[i];
	}
	buf[i] = '\0';
	String_Clear((char *)USB_USART_RX_BUF,strlen((const char *)USB_USART_RX_BUF));
	USB_USART_RX_STA = 0;
}
/*******************************************************************************
* �� �� ��         : PC_StartPam
* ��������		     : ��Ƭ������Է���������������
* ��    ��         : txt�����������ı�
* ��    ��         : ��
*******************************************************************************/
void PC_StartPam(char *txt)
{
	int i;
	String_Clear(SendBuf, strlen(SendBuf));
	PAM8406_Open();
	strcpy(SendBuf,"StartPam%");
	strcat(SendBuf,txt);		//�����ַ����ϲ�
	strcat(SendBuf,"%$$");
	PC_SendData(SendBuf);
	for(i=0;i<7;i++)
	{
		delay_ms(1431);	
	}
	PAM8406_Close();
}
/*******************************************************************************
* �� �� ��         : PC_Start
* ��������		     : ʹ��USB���ߴ���ģʽ�����ͨѶ�ж��Ƿ�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void PC_Start(void)
{
	char i;
	u8 pcisbreak, Sendtimes;
	
	PC_Connecting = 1;
	PC_RestartFlag = 0;
	Sendtimes = 0;
	pcisbreak =0;
	
	HDMIShowInfo("΢���������У����Եȡ�");
	//�ȵ��������꣬���Ի��ɽ��������ɹ�֮�����ַ����ж�
	for(i=0;i<31;i++)
	{
			delay_ms(941);
	}
	while (Sendtimes < 3)
	{
		delay_ms(100);
		PC_SendData("Hello%$$");
		Sendtimes++;
		for (i=0; i<20; i++)  //�ȴ���������
		{
			if ((usart2_rcv_len > 0) || (USB_USART_RX_STA > 0)) 
			{
				delay_ms(341);
				if ((NULL != strstr((const char *)usart2_rcv_buf, (const char *)"Too"))||(NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"Too")))
				{
					delay_ms(100);
					break;
				}				
			}
			delay_ms(600);
		}
		if (i<20)  
		{
				pcisbreak = 1;
				break;
		}
	}
	if(pcisbreak == 1)
	{
		PC_Connecting = 0;
		PC_RestartFlag = 0;
		HDMIShowInfo("�豸�����ɹ�");
	}
	else
	{
		PC_Connecting = 1;
		HDMIShowInfo("ϵͳ�����С�");
		PC_ShutDown();
		for(i=0;i<31;i++)
		{
			delay_ms(941);
		}
		PC_RestartFlag = 1;  //����������־λ
	}
}

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
