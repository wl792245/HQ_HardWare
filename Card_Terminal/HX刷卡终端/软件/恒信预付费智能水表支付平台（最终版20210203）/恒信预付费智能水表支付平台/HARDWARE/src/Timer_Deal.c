/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Timer_Deal.c
 * ��Ŀ���� ����ʱ�����������
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "Timer_Deal.h"

#include "String_Get.h"
#include "AT24C02.h"
#include "string.h"
#include "M6311.h"
#include "systick.h"
#include "HDMI_T050.h"
#include "XFS.h"
#include "CardInfoDeal.h"
///��ʱ�ж�3��ʱ��1�롣
///����600��SOCKET������M6311����
volatile u32  sec=0;
volatile u32  err=0;
volatile uint32_t   TimeIsInter=0; //��ʱ���жϱ�־λ

static char UploadTimerBuf[50];
static char TimeSendBuf[50];
/*******************************************************************************
* �� �� ��         : FirstHeartOpen
* ��������		     : ����Ϣ���������͵�һ������	   
* ��    ��         : ������TerminalId
* ��    ��         : ��
*******************************************************************************/
void FirstHeart_Open(char *TerminalId)
{	
	u8  i, Sendtimes;
	u8 netisbreak;
	String_Clear(TimeSendBuf, 50);
	String_Clear(UploadTimerBuf, 50);
	
	strcpy(TimeSendBuf,"TSendheart%");	//��һ������
	strcat(TimeSendBuf,"FirstHeart");
	strcat(TimeSendBuf,TerminalId);
	strcat(TimeSendBuf,"%$$");
	
  strcpy(UploadTimerBuf, TimeSendBuf);
	
	Sendtimes = 0;
	netisbreak =0;
	while (Sendtimes < 3)
	{
			delay_ms(100);
			M6311_SendData(UploadTimerBuf,strlen(UploadTimerBuf));
		  Sendtimes++;
			for (i=0; i<20; i++)  //�ȴ���������
			{
				if (usart2_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"K"))
					{
						delay_ms(100);
						break;
					}				
				}
				delay_ms(600);
			}
			if (i<20)  
			{
				  netisbreak = 1;
					break;
			}
	}
	if(netisbreak == 1)
	{
		M6311_SendData("HeartOK", strlen("HeartOK"));
	}
	else
	{
		CPU_Reset();
	}

}

/*******************************************************************************
* �� �� ��         : TIM_Heart_Deal
* ��������		     : ��ʱ���ִ���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void TIM_Heart_Deal(void)
{
	u8 i,Sendtimes;
	u8 netisbreak;
	String_Clear(TimeSendBuf, 50);
	String_Clear(UploadTimerBuf, 50);
	strcpy(TimeSendBuf,"TSendheart%");
	strcat(TimeSendBuf,DeviceInfo.TerminalInfo.TerminalId);
	strcat(TimeSendBuf,"v5");
	strcat(TimeSendBuf,"%$$");
	strcpy(UploadTimerBuf, TimeSendBuf);
	Sendtimes = 0;
	netisbreak = 0;
	while (Sendtimes < 3)
	{
			M6311_SendData(UploadTimerBuf, strlen(UploadTimerBuf));
		  Sendtimes++;
			for (i=0; i<20; i++)  //�ȴ���������
			{
				if (usart2_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"K"))
					{
						delay_ms(100);
						break;
					}
				}
				delay_ms(600);
			}
			if (i<20)  
			{
				netisbreak = 1;
				break;
			}
	}
  if(netisbreak == 1)
	{
		M6311_SendData("HeartOK", strlen("HeartOK"));
	}
	else
	{
		XFS_Fail_Play("��ʱ������ʧ�ܣ���Ⱥ�");
		CPU_Reset();
	}
  CacheOrder_update();
}














/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
