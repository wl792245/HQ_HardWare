/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��CardInfoDeal.c
 * ��Ŀ���� ���Ի��񿨽�����Ϣ��ȡ�봦��    
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "CardInfoDeal.h"
#include "PC_Config.h"
#include "USB_Config.h"
#include "HDMI_T050.h"
#include "systick.h"
#include "hw_config.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"
#include "string.h"
#include "String_Config.h"
#include "AT24C02.h"
#include "M6312.h"
#include "stdlib.h"

volatile u8  CardCommand=0;      //��ָ��

#define RCVLEN   250             //���ճ���

char RecievePCBuf[RCVLEN];       //���յ��Է��صĽ��
char SendPCBuf[RCVLEN];          //���Է���д������

char SendServerBuf[RCVLEN];      //���͵���������
char RecieveServerBuf[RCVLEN];   //���շ��������صĽ��


//char ServerRecHead[18];                 //������ȡͷ
//char ServerRecStr[200];                 //����������
HXCard4428_Info HXCard4428_RecvInfo;    //����4428��������Ϣ�ṹ��

#if 1
/*******************************************************************************
* �� �� ��         : HXCard_Deal
* ��������		     : ����ͨ����PC���Ͷ��������ȡ���ݽ��д���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
//void HXCard_Deal(void)
//{
//	u8 Sendtimes, netisbreak, pcisbreak;
//	u8 i,Char_Time;
//	u32 num[20];
//	u32 num1[20];
//	//char CardKind[10] = {0};//�����������
//	
//	Sendtimes = 0;
//	Char_Time = 0;
//	netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�����
//	pcisbreak = 0;                      //�жϵ�ǰ����ͨѶ�Ƿ�����
//	while (Sendtimes < 3)
//	{
//		PC_ReadCard();	//����������
//		delay_ms(100);
//		Sendtimes++;
//		for (i=0; i<20; i++)  //�ȴ���������
//		{
//			if ((usart2_rcv_len > 0) || (USB_USART_RX_STA > 0)) 
//			{
//				delay_ms(341);
//				if ((NULL != strstr((const char *)usart2_rcv_buf, (const char *)"%"))||(NULL != strstr((const char *)USB_USART_RX_BUF, (const char *)"%")))
//				{
//					delay_ms(100);
//					break;
//				}				
//			}
//			delay_ms(600);
//		}
//		if (i<20)  
//		{
//			  pcisbreak = 1;                      //�жϵ�ǰ����ͨѶ�Ƿ�����
//			  String_Clear(RecievePCBuf,RCVLEN);
//				if(usart2_rcv_len > 0)
//				{
//					PC_Usart_GetRcvData(RecievePCBuf, usart2_rcv_len);
//				}
//				if (USB_USART_RX_STA > 0)
//				{
//					PC_USB_GetRcvData(RecievePCBuf, USB_USART_RX_STA);
//				}
//				Char_Time = Char_Num(RecievePCBuf, '$');
//				if (Char_Time > 1)
//				{
//					break;
//				}
//		}
//		else
//		{
//			pcisbreak = 0;
//		}
//	}
//	if (pcisbreak == 0)
//	{
//			//�����ο�΢����û�����ݷ��أ�ϵͳ��λ
//			//�ȴ�΢�����Զ��ػ�
//			//bitCardOff = 0x68;  //���ο�����
//			//USB_PC_SendData("CardOff%$$");                     //USB��ʽ
//			//USART_PC_SendData("CardOff%$$", strlen("CardOff%$$"));    //����ģʽ
//		  PC_SendData("CardOff%$$");
//			HDMIShowInfo("��ȡ��,3���Ӻ��ٲ忨");
//		  PC_Close();
//			for(i=0;i<210;i++)
//			{
//				delay_ms(1024);
//			}
//		  CPU_Reset();
//	}
//	else
//	{
//		//��ȷ��������
//		if (Sendtimes < 3)
//		{
//			//bitCardOff=0x68;
//			PC_SendData("CardOff%$$");
//			Interger_Clear(num,20);
//			Char_Find(num,RecievePCBuf, '%');   
//			String_Find(CardKind, RecievePCBuf, 0, num[0]);                  //ȡͷ
//			if (strcmp(CardKind,"4442")==0) //4442�����ݴ���,4442������14λ��ȡ��8λ��
//			{
//				
//			}
//			else if (strcmp(CardKind,"4428")==0)//4428�����ݴ���
//			{
//				String_Find(HXCard4428_RecvInfo.Value14428, RecievePCBuf, num[0]+1, num[1]);     //������
//				String_Find(HXCard4428_RecvInfo.SByStrHX, RecievePCBuf, num[1]+1, num[2]);       //������
//				if (strcmp(HXCard4428_RecvInfo.Value14428,"6")== 0)                              //4428����������
//				{
//					/*ϵͳ��|�û����|*/
//					Interger_Clear(num1,20);
//					Char_Find(num1,HXCard4428_RecvInfo.SByStrHX, '|');   
//					String_Find(HXCard4428_RecvInfo.HX4428Sysnum, HXCard4428_RecvInfo.SByStrHX, 0, num1[0]); // ϵͳ��
//					String_Find(HXCard4428_RecvInfo.HX4428Userid, HXCard4428_RecvInfo.SByStrHX, num1[0]+1, num1[1]); // �û���
//					String_Clear(SendServerBuf,RCVLEN);
//					strcpy(SendServerBuf,"Hx4428USERLCID%");
//					strcat(SendServerBuf,DeviceInfo.TerminalInfo.TerminalId);
//					strcat(SendServerBuf,"%");
//					strcat(SendServerBuf,HXCard4428_RecvInfo.HX4428Userid);
//					strcat(SendServerBuf,"%");
//					strcat(SendServerBuf,HXCard4428_RecvInfo.HX4428Sysnum);
//					strcat(SendServerBuf,"%0");
//					strcat(SendServerBuf,"%$$");
//					//ͨ����������ѯ����
//					Sendtimes = 0;
//					Char_Time = 0;
//					while (Sendtimes < 3)
//					{
//						M6312_SendData(SendServerBuf, strlen(SendServerBuf));                       //ͨ��M6312�������������
//						Sendtimes++;
//						for (i=0; i<20; i++)  //
//						{
//							if (usart1_rcv_len > 0) 
//							{
//								if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"%"))
//								{
//									delay_ms(100);
//									break;
//								}
//							}
//							delay_ms(341);
//						}
//						if (i < 20)
//						{
//							netisbreak = 1;                                                   //��ǰ����ͨѶ����
//							String_Clear(RecieveServerBuf, 250);
//							M6312_USART_GetRcvData(RecieveServerBuf, usart1_rcv_len);
//							Char_Time = Char_Num(RecieveServerBuf, '$');
//							if (Char_Time > 1)
//							{
//								break;
//							}
//						}
//						else
//						{
//							netisbreak = 0;
//						}
//					}
//					if (netisbreak == 0)
//					{
//						 CardCommand=0x41;	//M6312�ϵ����ϵ�������
//					}
//					else
//					{
//						if (Sendtimes < 3)
//						{
//							
//						}
//						else
//						{
//							CardCommand = 0x68;
//								//PC_SendData("CardOff%$$");
//								//HDMIShowInfo("����ˢ");
//								//PC_StartPam("����ˢ");
//						}
//					}
//				}
//				else if (strcmp(HXCard4428_RecvInfo.Value14428,"1")== 0)                         //4428�û�������
//				{
//					
//				}
//				else
//				{
//						PC_SendData("CardOff%$$");
//						PC_StartPam("δ֪��,����ȷ�忨��");
//						HDMIShowInfo("δ֪��,����ȷ�忨��");
//				}
//			}
//			else
//			{
//				//PC_SendData("CardOff%$$");
//				//PC_StartPam("δ֪��,����ȷ�忨��");
//				//HDMIShowInfo("δ֪��,����ȷ�忨��");
//				//CardCommand = 
//			}
//		}

//		else
//		{
//			//�����ݲ���ȷ
//			CardCommand = 0x68;  //���ο�����
//			//PC_SendData("CardOff%$$");
//			//HDMIShowInfo("����ˢ");
//			//PC_StartPam("����ˢ");
//		}
//	}

//}
#endif
/*******************************************************************************
* �� �� ��         : SCCard_Deal
* ��������		     : ������ͨ����PC���Ͷ��������ȡ���ݽ��д���
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void SCCard_Deal(void)
{
  u8 PC_ReturnValue, PC_SpitValue, M6312_ReturnValue;
  SCCard_5557PC SCCard_PCInfo;
	SCCard_Server SCCard_ServerInfo;
	
	PC_ReturnValue = 0;
	PC_SpitValue = 0;
	M6312_ReturnValue = 0;
	
	PC_RestartFlag = 0;
	M6312_RestartFlag = 0;
	//��PC���Ͷ�����Ϣ
	PC_ReturnValue = PC_ReadData(RecievePCBuf, 3, RCVLEN);
	//����ͨѶ�ɹ�
	if (PC_ReturnValue == 1)
	{
      PC_SpitValue = SCCard_PCDataSpit(&SCCard_PCInfo, SendServerBuf,RecievePCBuf);
		  //���Է���������ȷ
			if (PC_SpitValue == 1)
			{
				//�����������ͨѶ
				M6312_ReturnValue = M6312_UploadData(RecieveServerBuf, SendServerBuf, "%$$", 3,RCVLEN);
				//������ͨѶ�ɹ�
				if (M6312_ReturnValue == 1)
				{
					SCCard_ServerDataSpit(&SCCard_ServerInfo, SCCard_PCInfo, SendServerBuf, RecievePCBuf,RecieveServerBuf, SendPCBuf);
				}
				else
				{
					M6312_RestartFlag = 1;  //M6312����
				}
			}
			else if (PC_SpitValue == 0)
			{
					HDMIShowInfo("δ֪��,����ȷ�忨��");
				  delay_ms(10);
		     	PC_StartPam("δ֪��,����ȷ�忨��");
			}
			else if(PC_SpitValue == 2)
			{
				HDMIShowMenuInfo(&DeviceInfo);        //��������ʾ
				return ;
			}				
	}
	//����ͨѶʧ��
	else
	{
		  PC_StartPam("��ȡ��,3���Ӻ��ٲ忨");
			delay_ms(300);
			HDMIShowInfo("��ȡ��,3���Ӻ��ٲ忨");
		  PC_RestartFlag = 1;   //��������
	}
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/

