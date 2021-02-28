/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��SCCard_Config.c
 * ��Ŀ���� ��������ģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-2-3
**********************************************************************************/
#include "SCCard_Config.h"
#include "stm32f10x_exti.h"
#include "String_Config.h"
#include "string.h"
#include "HDMI_T050.h"
#include "systick.h"
#include "PC_Config.h"
#include "AT24C02.h"
#include "M6312.h"
volatile uint32_t   SCCardIsInter=0; //����5557���жϱ�־λ
/*******************************************************************************
* �� �� ��         : SCCard_Init
* ��������		     : ������ģ���ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void SCCard_Init(u16 m_irq, u16 s_irq)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		//����gpio�������忨�ж�,PB5
		GPIO_InitStructure.GPIO_Pin=SCCARD_INIT_CSW;	 //ѡ����Ҫ���õ�IO��(pin_5)
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	  //������������ģʽ
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
		GPIO_Init(SCCARD_INIT_Port,&GPIO_InitStructure); /* ��ʼ��GPIO */	
		/*��GPIO�ܽ����ⲿ�ж�������*/
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);//ѡ��GPIO�ܽ������ⲿ�ж���·
		/*����EXTI*/
		EXTI_InitStructure.EXTI_Line=SCCARD_Line;
		EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;//�½��ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure); 
		
		/* ����NVIC���� */	
		NVIC_InitStructure.NVIC_IRQChannel = SCCARD_INIT_IRQn;		//��EXTI1��ȫ���ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //��ռ���ȼ�Ϊ1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //��Ӧ���ȼ�Ϊ2
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //ʹ��
		NVIC_Init(&NVIC_InitStructure);	
}
/*******************************************************************************
* �� �� ��         : SCCard_PCDataSpit
* ��������		     : �������������ݷֽ�
* ��    ��         : SCCardPC:д����Է��ص�����  pdst��д��������������� psrc���ֽ�����յ�������
* ��    ��         : ��
*******************************************************************************/
u8 SCCard_PCDataSpit(PSCCard_5557PC SCCardPC,char *pdst_ServerSend,char *psrc_PCRecv)
{
	  u32 num[20];
		Interger_Clear(num,20);
		String_Clear(SCCardPC->PCRecHead, 2);
		String_Clear(SCCardPC->PCRecStr,200);
		Char_Find(num,psrc_PCRecv, '%');   
		String_Find(SCCardPC->PCRecHead, psrc_PCRecv, 0, num[0]);                  //ȡͷ
	  if (strcmp(SCCardPC->PCRecHead, "1") == 0)                                  //dll���ض���ֵ
		{
			String_Find(SCCardPC->PCRecStr, psrc_PCRecv, num[0]+1, num[1]);            // ������
			Interger_Clear(num,20);
			Char_Find(num,SCCardPC->PCRecStr, '|');
			String_Find(SCCardPC->iCardTypeSC, SCCardPC->PCRecStr, 0, num[0]);                // ����5557������
			String_Find(SCCardPC->iManageCodeSC, SCCardPC->PCRecStr, num[0]+1, num[1]);       // ����5557��ϵͳά����
			String_Find(SCCardPC->iKeySC, SCCardPC->PCRecStr, num[1]+1, num[2]);              // ����5557����Կ
			String_Find(SCCardPC->CardIdSC, SCCardPC->PCRecStr, num[2]+1, num[3]);            // ����5557������
			String_Find(SCCardPC->BiaoIdSC, SCCardPC->PCRecStr, num[3]+1, num[4]);            // ����5557�����
			String_Find(SCCardPC->BuyNumSC, SCCardPC->PCRecStr, num[4]+1, num[5]);            // ����5557����ֵ���
			String_Find(SCCardPC->CardMonenySC, SCCardPC->PCRecStr, num[5]+1, num[6]);        // ����5557���ܳ�ֵ���
			String_Find(SCCardPC->BuyAllNumSC, SCCardPC->PCRecStr, num[6]+1, num[7]);         // ����5557���ܳ�ֵ�����
			String_Find(SCCardPC->UseWaterSC, SCCardPC->PCRecStr, num[7]+1, num[8]);          // ����5557����ˮ����0.1�֣�
			String_Find(SCCardPC->ZhuangTaiMaSC, SCCardPC->PCRecStr, num[8]+1, num[9]);       // ����5557��״̬��
				//�¹�ˮδȦ�棬����ˢ��
			if ((strcmp(SCCardPC->iCardTypeSC, "288") == 0)||(strcmp(SCCardPC->iCardTypeSC, "298") == 0))
			{
				return 2;
			}
			else
			{
				String_Clear(pdst_ServerSend,250);
				strcpy(pdst_ServerSend,"SANC5557PCID%");
				strcat(pdst_ServerSend,DeviceInfo.TerminalInfo.TerminalId);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->CardIdSC);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->iManageCodeSC);
				strcat(pdst_ServerSend,"%");
				strcat(pdst_ServerSend,SCCardPC->CardMonenySC);
				strcat(pdst_ServerSend,"%$$");
				return 1;
			}
		}
		else
		{
			  return 0;
		}
		
}
/*******************************************************************************
* �� �� ��         : SCCard_ServerDataSpit
* ��������		     : ���������������ݷֽ�
* ��    ��         : SCCardServer:д����������ص�����  SCCardPC:������Է��ص����� pdst_ServerSend���������������� pdst_PCRec: ���Խ������ݣ� psrc���ֽ�����Դ writecard:д��д������
* ��    ��         : ��
*******************************************************************************/
void SCCard_ServerDataSpit(PSCCard_Server SCCardServer,const SCCard_5557PC SCCardPC, char *pdst_ServerSend, char *pdst_PCRec, char *psrc_Server, char * PCSend)
{
	  u32 num[20];
	  char  PC_ReturnHead[3];
	  CardType_Info CardType = SC5557;
	  u8 PC_ReturnValue, M6312_ReturnValue;
	  
	  UserInfoShow  UserInfo;
	  PC_ReturnValue = 0;
	  M6312_ReturnValue = 0;
	  Get_Head(SCCardServer->ServerRecHead,psrc_Server, '%', num, 20);
	  //ͷ�ж�
    if(strcmp(SCCardServer->ServerRecHead,"SC5557USERP")==0)	                //����5557������������
		{
			String_Find(SCCardServer->SCID, psrc_Server, num[0]+1, num[1]);                // ȡ�豸��
			String_Find(SCCardServer->SC5557CardID, psrc_Server, num[1]+1, num[2]);        // ȡ����
			String_Find(SCCardServer->SC5557OrderId, psrc_Server, num[2]+1, num[3]);       // ȡ������
			String_Find(SCCardServer->iPrice1SC, psrc_Server, num[3]+1, num[4]);           // ȡһ��
			String_Find(SCCardServer->iPrice2SC, psrc_Server, num[4]+1, num[5]);           // ȡ����
			String_Find(SCCardServer->iPrice3SC, psrc_Server, num[5]+1, num[6]);           // ȡ����
			String_Find(SCCardServer->iTon1SC, psrc_Server, num[6]+1, num[7]);             // ȡһ��
			String_Find(SCCardServer->iTon2SC, psrc_Server, num[7]+1, num[8]);             // ȡ����
			String_Find(SCCardServer->BuyMonenySC, psrc_Server, num[8]+1, num[9]);         // ȡ��ֵ���
			
		  String_Clear(PCSend, 250);
			strcpy(PCSend,SCCardPC.BiaoIdSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->BuyMonenySC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardPC.BuyNumSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardPC.BuyAllNumSC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iTon1SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iTon2SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice1SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice2SC);
			strcat(PCSend,"|");
			strcat(PCSend,SCCardServer->iPrice3SC);
			strcat(PCSend,"|");
			//д��
			PC_ReturnValue = PC_WirteData(pdst_PCRec,PCSend,3,250,CardType);
			//д���ɹ������յ�PC���ص�����
			if (PC_ReturnValue == 1)
			{
				Get_Head(PC_ReturnHead ,pdst_PCRec, '%', num, 20);
				//д���ɹ�
				if (strcmp(PC_ReturnHead, "1") == 0)
				{
					SCCard_ServerDataCombine(pdst_ServerSend, *SCCardServer, PC_ReturnHead,1);
					M6312_ReturnValue = M6312_UploadData(psrc_Server, pdst_ServerSend, "OK", 3, 250);
					if (M6312_ReturnValue == 1)
					{
						if ((strcmp(psrc_Server, "updateorderstateOK2")==0) || (strstr(psrc_Server, "pdateorderstateOK2")!= NULL))
						{
							//��ֵ�ɹ�
							HDMIShowRechargeSucess(SCCardServer->BuyMonenySC, 0);
							Result_StartPam(SCCardServer->BuyMonenySC, 0);
						}
						else if (strcmp(psrc_Server,"updateorderstateOK1")==0)
						{
							HDMIShowInfo("���׹رգ���Ӫҵ������");
							PC_StartPam("���׹رգ���Ӫҵ������");
						}
					}						
				}
				else 
				{
					HDMIShowInfo("��ֵʧ�ܣ�������ˢ��");
					delay_ms(10);
					PC_StartPam("��ֵʧ�ܣ�������ˢ��");
					SCCard_ServerDataCombine(pdst_ServerSend, *SCCardServer, PC_ReturnHead,0);
					M6312_SendData(pdst_ServerSend, strlen(pdst_ServerSend));
				}
			}
			//δ���յ�PC���ص�����
			else
			{
				HDMIShowInfo("��ֵʧ�ܣ�������ˢ��");
				PC_StartPam("��ֵʧ�ܣ�������ˢ��");
			}
		}
		else if (strcmp(SCCardServer->ServerRecHead,"SC5557USERPcheck")==0) //����5557��ѯ������SC5557USERPcheck
		{
			String_Find(SCCardServer->SCID, psrc_Server, num[0]+1, num[1]);                // ȡ�豸��
			String_Find(SCCardServer->SC5557CardID, psrc_Server, num[1]+1, num[2]);        // ȡ����
			String_Find(SCCardServer->SC5557UserID, psrc_Server, num[2]+1, num[3]);        // ȡ�û���
			String_Find(SCCardServer->SCAdress, psrc_Server, num[3]+1, num[4]);            //ȡ�û���ַ
			String_Find(SCCardServer->SCXingzhi, psrc_Server, num[4]+1, num[5]);           //ȡ��ˮ����
			String_Find(SCCardServer->SCTotalamount, psrc_Server, num[5]+1, num[6]);       //ȡ�û�ˮ����ˮ������
			String_Find(SCCardServer->SCLastAllBuyMount, psrc_Server, num[6]+1, num[7]);   //ȡ�û������ѹ�ˮ����
			UserInfo.UserInfo.userId = SCCardServer->SC5557UserID;
			UserInfo.UserInfo.userAdress = SCCardServer->SCAdress;
			UserInfo.WaterInfo.waterProperties = SCCardServer->SCXingzhi;
			UserInfo.WaterInfo.totalAmount = SCCardServer->SCTotalamount;
			UserInfo.WaterInfo.LastAllBuyMount = SCCardServer->SCLastAllBuyMount;
			UserInfo.WaterInfo.Unit = 1;
			if (strlen(SCCardServer->SCAdress) > 18)
			{
				UserInfo.islongthan12 = 1;
			}
			else
			{
				UserInfo.islongthan12 = 0;
			}
			HDMIShowUserDetail(UserInfo);
			PC_StartPam("΢�ŽɷѺ��뽫��ȡ�������²������д������");
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
			delay_ms(1023);
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr0")==0)//δ֪�û�����Ӫҵ������
		{
			HDMIShowInfo("δ֪�û�����Ӫҵ������");
			PC_StartPam("δ֪�û�����Ӫҵ������");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557Perr1")==0)//��΢��֧������ˢ��
		{
			HDMIShowInfo("��΢��֧������ˢ��");
			PC_StartPam("��΢��֧������ˢ��");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr2")==0)//��΢��֧������ˢ��
		{
			HDMIShowInfo("��΢��֧������ˢ��");
			PC_StartPam("��΢��֧������ˢ��");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557err3")==0)//��Ч������Ӫҵ������
		{
			HDMIShowInfo("��Ч������Ӫҵ������");
			PC_StartPam("��Ч������Ӫҵ������");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SC5557USERPerr12")==0)//���׹رգ���Ӫҵ������
		{
			HDMIShowInfo("���׹رգ���Ӫҵ������");
			PC_StartPam("���׹رգ���Ӫҵ������");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"WebserviceErro")==0)
		{
			HDMIShowInfo("�������δ����");
			PC_StartPam("�������δ����");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"WebNoAnswer")==0)
		{
			HDMIShowInfo("��������Ӧ");
			PC_StartPam("��������Ӧ");
		}
		else if(strcmp(SCCardServer->ServerRecHead,"SANC5557PCID")==0)
		{
			
		}
		else
		{
		}

}
/*******************************************************************************
* �� �� ��         : SCCard_ServerDataCombine
* ��������		     : �������������������
* ��    ��         : pdst_ServerSend:��������������  SCCardServer:�������������� WriteCardSuccess:д���ɹ����� isSuccess:�Ƿ�д���ɹ�
* ��    ��         : ��
*******************************************************************************/
void SCCard_ServerDataCombine(char *pdst_ServerSend, const SCCard_Server SCCardServer, char * WriteCardSuccess, int isSuccess)
{
	String_Clear(pdst_ServerSend, 250);
	strcpy(pdst_ServerSend,"SANC5557Result%");
	strcat(pdst_ServerSend,SCCardServer.SCID);
	strcat(pdst_ServerSend,"%");
	strcat(pdst_ServerSend,SCCardServer.SC5557CardID);
	strcat(pdst_ServerSend,"%");
	strcat(pdst_ServerSend,SCCardServer.SC5557OrderId);	
	if (isSuccess == 1)
	{
		strcat(pdst_ServerSend,"%Success%$$");
	}
	else if (isSuccess == 0)
	{
		strcat(pdst_ServerSend,"%Fail%$$");
	}
	strcat(pdst_ServerSend,WriteCardSuccess);
}





/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
