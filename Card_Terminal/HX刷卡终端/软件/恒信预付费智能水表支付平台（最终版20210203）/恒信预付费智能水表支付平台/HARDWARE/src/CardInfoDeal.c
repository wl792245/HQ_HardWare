/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��CardInfoDeal.c
 * ��Ŀ���� ����24����57����42��������Ϣ��ȡ�봦��    
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "CardInfoDeal.h"
#include "stm32f10x_exti.h"
#include "string.h"


#include "M6311.h"
#include "systick.h"
#include "AT24C02.h"
#include "XFS.h"
#include "HDMI_T050.h"
#include "utils.h"
#include "String_Get.h"

volatile uint32_t   CardIsInter=0; //42.24���жϱ�־λ

UserInfoShow        UserInfoDisplay;   //�û�����ˮ��Ϣ�ṹ��
Card_DealSend       Card_SendInfo;     //��������Ϣ�ṹ��
Card_DealRecv       Card_RecvInfo;     //��������Ϣ�ṹ��

char UploadServerBuf[200];    //�ϴ�����������
char RecieveServerBuf[200];   //���շ��������صĽ��
char voicesucc[30];           // ���ڲ��ųɹ�
u32 num[20];                     //���ҷָ����ĸ���

int islongthan12 = 0;                  //�ַ����ȳ���12
u32 Char_Time = 0;                     //�����ַ��ظ�����
u8 Sendtimes = 0;                      //�ϴ������������ݴ���
u8 netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ
/*******************************************************************************
* �� �� ��         : Card_PowerInit
* ��������		     : ��24��.42����Դ���ų�ʼ��  
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void Card_PowerInit(void)	  
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); /* ����GPIOʱ�� */
	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=Card_POWER;				//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		//�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;		//���ô�������
	GPIO_Init(Card_GPIO,&GPIO_InitStructure); /* ��ʼ��GPIO */	
}
/*******************************************************************************
* �� �� ��         : Card_Inter
* ��������		     : 24��.42���ж�����
* ��    ��         : m_irq����ռ���ȼ���1����s_irq����Ӧ���ȼ���0��
* ��    ��         : ��
*******************************************************************************/
static void Card_Inter(u16 m_irq, u16 s_irq)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=Card_ICSW ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(Card_GPIO,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9);//ѡ��GPIO�ܽ������ⲿ�ж���·
	//�˴�һ��Ҫ��ס���˿ڹܽż����ж��ⲿ��·
	/* �����ⲿ�жϵ�ģʽ */ 
	EXTI_InitStructure.EXTI_Line=EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* ����NVIC���� */	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
	NVIC_InitStructure.NVIC_IRQChannel = Card_IRQn; 	//��EXTI2��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //��ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //��Ӧ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //ʹ��
	NVIC_Init(&NVIC_InitStructure); 
}	
/*******************************************************************************
* �� �� ��         : Card_Init
* ��������		     : 24��.42����ʼ�� 
* ��    ��         : m_irq����ռ���ȼ���1����s_irq����Ӧ���ȼ���0��
* ��    ��         : ��
*******************************************************************************/
void Card_Init(u16 m_irq, u16 s_irq)
{
	I2C_INIT();
	Card_PowerInit();
	Card_Inter( m_irq, s_irq);
}
/*******************************************************************************
* �� �� ��         : CardCIDCopy24C02
* ��������		     : ��24���Ļ�����IICID���Ƶ��������ϵ�24c02  5-37 32λ
* ��    ��         : ��
* ��    ��         : 0:���Ƴɹ���1������ʧ��
*******************************************************************************/
u8 CopyCIDTo24C02(void)
{
	char ConfigurationCardInfo[32];
	unsigned int Num[5] = {0};
	Card24_Read(5,ConfigurationCardInfo,32);
	Char_Find(Num, ConfigurationCardInfo, '%');
	delay_ms(100);
	if((Num[0] == 10) &&(Num[1] == 31))	
	{
		while(AT24C02_Write(5,ConfigurationCardInfo,32))
		{
			return 1;
		}
		return 0;
	}
	else
	{
		return 1;
	}
}
/*******************************************************************************
* �� �� ��         : CopyScrrenvTo24C02
* ��������		     : ��24������Ļ�汾�Ÿ��Ƶ��������ϵ�24c02  37-42 5λ
* ��    ��         : ��
* ��    ��         : 0:���Ƴɹ���1������ʧ��
*******************************************************************************/
u8 CopyScrrenvTo24C02(void)
{
	char ConfigurationCardInfo[5];
	Card24_Read(37,ConfigurationCardInfo,5);
	delay_ms(100);
	while(AT24C02_Write(37,ConfigurationCardInfo,5))
	{
			return 1;
	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : CopyTerminalVTo24C02
* ��������		     : ��24�����豸�汾�Ÿ��Ƶ��������ϵ�24c02 42-47 5λ
* ��    ��         : ��
* ��    ��         : 0:���Ƴɹ���1������ʧ��
*******************************************************************************/
u8 CopyTerminalVTo24C02(void)
{
	char ConfigurationCardInfo[5];
	Card24_Read(42,ConfigurationCardInfo,5);
	delay_ms(100);
	while (AT24C02_Write(42,ConfigurationCardInfo,5))
	{
		return 1;
	}
	return 0;

}
/*******************************************************************************
* �� �� ��         : CopyIPPortTo24C02
* ��������		     : ��24���ķ�������ַ�˿ںŸ��Ƶ��������ϵ�24c02 47-69 22λ
* ��    ��         : ��
* ��    ��         : 0:���Ƴɹ���1������ʧ��
*******************************************************************************/
u8 CopyIPPortTo24C02(void)
{
	char ConfigurationCardInfo[22];
	Card24_Read(47,ConfigurationCardInfo,22);
	delay_ms(100);
	while (AT24C02_Write(47,ConfigurationCardInfo,22))
	{
		return 1;
	}
	return 0;

}
/*******************************************************************************
* �� �� ��         : CopyQRcodeTo24C02
* ��������		     : ��24���Ķ�ά�븴�Ƶ��������ϵ�24c02 69-90 21λ
* ��    ��         : ��
* ��    ��         : 0:���Ƴɹ���1������ʧ��
*******************************************************************************/
u8 CopyQRcodeTo24C02(void)
{
	char  ConfigurationCardInfo[21];
	Card24_Read(69,ConfigurationCardInfo,21);
	delay_ms(100);
	while(AT24C02_Write(69,ConfigurationCardInfo,21))
	{
		return 1;
	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : Card24_PDeal
* ��������		     : 24��Ԥ���Ѵ�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card24_PDeal(Card24_PreInfo *Info24p)
{
		Card24_PreInfo      SInfo24pre;         //24��Ԥ������Ϣ�ṹ��
	
	  u8  RecvState = 0;

    islongthan12 = 0;                  //�ַ����ȳ���12
		netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ
		
		Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info24p->cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
		HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
		Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info24p->areacode, 6);//��3�ֽ�����Ų�ֳ�6�����ֽ��������ת�����ַ�
		HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 6);//��4�����ֽ����������ת���ַ�����
		
		strcpy(Card_SendInfo.TSendCID,"TSend24PCID%");                                    //24Ԥ���ѿ�CID
		strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId);               //24Ԥ���ѿ�������
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //24Ԥ���ѿ�����
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //24Ԥ���ѿ������
		strcat(Card_SendInfo.TSendCID,"%$$");
		strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
		netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
		if(netisbreak==0)     //ͨѶʧ��
		{
			XFS_Fail_Play("24Ԥ����ͨѶʧ�ܣ�������");
			CPU_Reset();
		}
		else if (netisbreak == 1)                //ͨѶ����
		{
					Interger_Clear(num,20);
					Char_Find(num,RecieveServerBuf, '%');                                          //��ȡ�ٷֺŵ�λ��
					String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //ȡͷ
					if (strcmp(Card_RecvInfo.head,"SSend24P")==0)                                  //�ж�Ϊ24Ԥ���ѿ�	
					{
							String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
							String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
							String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //ȡ������
							String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //ȡ��ֵ��
							String_Find(Card_RecvInfo.times, RecieveServerBuf, num[4]+1, num[5]);          //ȡ����
							String_Find(Card_RecvInfo.security, RecieveServerBuf, num[5]+1, num[6]);        //ȡ����λ
							String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[6]+1, num[7]);  //ȡ���γ�ֵ��������������
							/*���������´����ݱ�������*/
							strcpy(Card_SendInfo.TSendCID,"TSend24PResult%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
							strcat(Card_SendInfo.TSendCID,"%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
							strcat(Card_SendInfo.TSendCID,"%");
							strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
						
							Card24_PreReadCID(&SInfo24pre);			  //��24Ԥ���ѿ�
							Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo24pre.cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
							HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
							if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)//�жϷ��������صĿ����Ƿ�͵�ǰ��һ��
							{
								strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
								strcpy((char *)Card_SendInfo.Card_CharInfo.strtimes, Card_RecvInfo.times);
								strcpy((char *)Card_SendInfo.Card_CharInfo.strsecurity, Card_RecvInfo.security);
								StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,2);//����ֵ���ַ���ת��Ϊʮ������
								StrToHex(Card_SendInfo.Card_HexInfo.hextime, Card_SendInfo.Card_CharInfo.strtimes,1);//�������ַ���ת��Ϊʮ������
								StrToHex(Card_SendInfo.Card_HexInfo.hexsecurity, Card_SendInfo.Card_CharInfo.strsecurity,2);//������λ�ַ���ת����ʮ�������ֽ�
								Card_SendInfo.Card_Flag = Card24_PreWriteSaleAmount(Card_SendInfo.Card_HexInfo.hexbuyamount,Card_SendInfo.Card_HexInfo.hexsecurity, Card_SendInfo.Card_HexInfo.hextime[0]);//д��
								if (Card_SendInfo.Card_Flag == 1)
								{
									strcat(Card_SendInfo.TSendCID,"%Success%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
									if (RecvState == 0)
									{
											XFS_Fail_Play("ͨ��ʧ�ܣ�����ˢ");     //��������������ˢ��
									}
									else if (RecvState == 1)
									{
											if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
											{
												String_Clear(voicesucc, 30);
												strcpy(voicesucc,"���ѳɹ���ֵ");
												strcat(voicesucc, Card_RecvInfo.pricemountstr);
												strcat(voicesucc,"��");
												XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
											}
											else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
											{
												XFS_Fail_Play("����δ��������ˢ");
											}
									}
								}
								else
								{
									XFS_Fail_Play("��ֵʧ��,����ˢ"); //������������Ļ��ʾ
									strcat(Card_SendInfo.TSendCID,"%Fail%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
								}														
							}
							else
							{
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
								XFS_Fail_Play("����ˢ");     //��������������ˢ��
							}
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Pcheck")==0)//�ж�Ϊ24Ԥ���ѿ�check
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
						String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //ȡ�û���
						String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //ȡ�û���ַ
						if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
						else islongthan12 = 0;
						String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //ȡ��ˮ����
						String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //ȡ�û�ˮ����ˮ������
						String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //ȡ�û������ѹ�ˮ��
						UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
						UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
						UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
						UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
						UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
						UserInfoDisplay.islongthan12 = islongthan12;
						UserInfoDisplay.WaterInfo.Unit = 1;
						HDMIShowUserDetail(UserInfoDisplay);
						XFS_Only_Play("΢�ŽɷѺ��뽫��ȡ�������²������д������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr0")==0)
					{
						XFS_Fail_Play("δ֪�û�����Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr1")==0)
					{
						XFS_Fail_Play("��΢��֧������ˢ��");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr3")==0)
					{
						XFS_Fail_Play("��Ч������Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend24Perr12")==0)
					{
						XFS_Fail_Play("���׹رգ���Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
					{
						XFS_Fail_Play("�������δ����");
					}
					else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
					{
						XFS_Fail_Play("��������Ӧ");
					}
					else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
					{
							char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
							AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
					}
					else
					{
						
					}
				
		}

}
/*******************************************************************************
* �� �� ��         : Card24_LDeal
* ��������		     : 24�����ݴ�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card24_LDeal(Card24_LadderInfo *Info24l)
{
	Card24_LadderInfo   SInfo24ladder;      //24��������Ϣ�ṹ��
	u8 price[8]= {'\0'};
	u8 amount[6]= {'\0'};

  u8  RecvState = 0;
		
	islongthan12 = 0;                  //�ַ����ȳ���12
  netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ
	

	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info24l->cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info24l->areacode, 6);//��3�ֽ�����Ų�ֳ�6�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 6);//��6�����ֽ����������ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info24l->watersalesamount, 8);//��4�ֽ���ˮ����ֳ�8�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 8);//��8�����ֽ�������ˮ��ת���ַ�����
	
	strcpy(Card_SendInfo.TSendCID,"TSend24LCID%");                                    //24���ݿ�CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId);               //24���ݿ�������
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //24���ݿ�����
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //24���ݿ������
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);//24���ݿ���ˮ��
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	if(netisbreak==0)     //ͨѶʧ��
	{
		XFS_Fail_Play("24����ͨѶʧ�ܣ�������");
		CPU_Reset();
	}
	else                 //ͨѶ����
	{
			Interger_Clear(num,20);
			Char_Find(num,RecieveServerBuf, '%');                                          //��ȡ�ٷֺŵ�λ��
			String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //ȡͷ
		  if (strcmp(Card_RecvInfo.head,"SSend24L")==0)                                  //�ж�Ϊ24���ݿ�	
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
				String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //ȡ������
				String_Find(Card_RecvInfo.oneprice, RecieveServerBuf, num[3]+1, num[4]);        //ȡһ��
				String_Find(Card_RecvInfo.twoprice, RecieveServerBuf, num[4]+1, num[5]);        //ȡ����
				String_Find(Card_RecvInfo.threeprice, RecieveServerBuf, num[5]+1, num[6]);      //ȡ����
				String_Find(Card_RecvInfo.fourprice, RecieveServerBuf, num[6]+1, num[7]);       //ȡ�ļ�
				String_Find(Card_RecvInfo.oneamount, RecieveServerBuf, num[7]+1, num[8]);       //ȡһ��
				String_Find(Card_RecvInfo.twoamount, RecieveServerBuf, num[8]+1, num[9]);       //ȡ����
				String_Find(Card_RecvInfo.threeamount, RecieveServerBuf, num[9]+1, num[10]);    //ȡ����
				String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[10]+1, num[11]);       //ȡ��ֵ��
				String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[11]+1, num[12]);  //ȡ���γ�ֵ��������������
				/*���������´����ݱ�������*/
				strcpy(Card_SendInfo.TSendCID,"TSend24LResult%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
				
				Card24_LadderReadCID(&SInfo24ladder);
				Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo24ladder.cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
				HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
				if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)//�жϷ��������صĿ����Ƿ�͵�ǰ��һ��
				{
					strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneprice, Card_RecvInfo.oneprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoprice, Card_RecvInfo.twoprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeprice, Card_RecvInfo.threeprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strfourprice, Card_RecvInfo.fourprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneamount, Card_RecvInfo.oneamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoamount, Card_RecvInfo.twoamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeamount, Card_RecvInfo.threeamount);
					
					StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,4);//����ֵ���ַ���ת��Ϊʮ������
					StrToHex(Card_SendInfo.Card_HexInfo.hexoneprice, Card_SendInfo.Card_CharInfo.stroneprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hextwoprice, Card_SendInfo.Card_CharInfo.strtwoprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexthreeprice, Card_SendInfo.Card_CharInfo.strthreeprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexfourprice, Card_SendInfo.Card_CharInfo.strfourprice, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexoneamount, Card_SendInfo.Card_CharInfo.stroneamount, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hextwoamount, Card_SendInfo.Card_CharInfo.strtwoamount, 2);
					StrToHex(Card_SendInfo.Card_HexInfo.hexthreeamount, Card_SendInfo.Card_CharInfo.strthreeamount, 2);
					price[0] = Card_SendInfo.Card_HexInfo.hexoneprice[0];
					price[1] = Card_SendInfo.Card_HexInfo.hexoneprice[1];
					price[2] = Card_SendInfo.Card_HexInfo.hextwoprice[0];
					price[3] = Card_SendInfo.Card_HexInfo.hextwoprice[1];
					price[4] = Card_SendInfo.Card_HexInfo.hexthreeprice[0];
					price[5] = Card_SendInfo.Card_HexInfo.hexthreeprice[1];
					price[6] = Card_SendInfo.Card_HexInfo.hexfourprice[0];
					price[7] = Card_SendInfo.Card_HexInfo.hexfourprice[1];
					amount[0] = Card_SendInfo.Card_HexInfo.hexoneamount[0];
					amount[1] = Card_SendInfo.Card_HexInfo.hexoneamount[1];
					amount[2] = Card_SendInfo.Card_HexInfo.hextwoamount[0];
					amount[3] = Card_SendInfo.Card_HexInfo.hextwoamount[1];
					amount[4] = Card_SendInfo.Card_HexInfo.hexthreeamount[0];
					amount[5] = Card_SendInfo.Card_HexInfo.hexthreeamount[1];
					
					Card_SendInfo.Card_Flag = Card24_LadderWrite(Card_SendInfo.Card_HexInfo.hexbuyamount, price, amount);//д��
					if (Card_SendInfo.Card_Flag == 1)
					{
						strcat(Card_SendInfo.TSendCID,"%Success%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
            if (RecvState == 0)
						{
							XFS_Fail_Play("ͨ��ʧ�ܣ�����ˢ");
						}
						else if (RecvState == 1)
						{
							if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
							{
								String_Clear(voicesucc, 30);
								strcpy(voicesucc,"���ѳɹ���ֵ");
								strcat(voicesucc, Card_RecvInfo.pricemountstr);
								strcat(voicesucc,"Ԫ");
								XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 0);
							}
							else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
							{
								XFS_Fail_Play("����δ��������ˢ");
							}
						}
					}
					else
					{
						XFS_Fail_Play("��ֵʧ��,����ˢ"); //������������Ļ��ʾ
						strcat(Card_SendInfo.TSendCID,"%Fail%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
					}									
				}
				else
				{
					strcat(Card_SendInfo.TSendCID,"%Fail%$$");
					strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
					M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
				  XFS_Fail_Play("����ˢ");
				}
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lcheck")==0)                          //�ж�Ϊ24���ݿ�check
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
				String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //ȡ�û���
				String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //ȡ�û���ַ
				if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
				else islongthan12 = 0;
				String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //ȡ��ˮ����
				String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //ȡ�û�ˮ����ˮ������
				String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //ȡ�û������ѹ�ˮ��
				UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
				UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
				UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
				UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
				UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
				UserInfoDisplay.islongthan12 = islongthan12;
				UserInfoDisplay.WaterInfo.Unit = 0;
				HDMIShowUserDetail(UserInfoDisplay);
				XFS_Only_Play("΢�ŽɷѺ��뽫��ȡ�������²������д������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr0")==0)
			{
				XFS_Fail_Play("δ֪�û�����Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr1")==0)
			{
				XFS_Fail_Play("��΢��֧������ˢ��");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr3")==0)
			{
				XFS_Fail_Play("��Ч������Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend24Lerr12")==0)
			{
				XFS_Fail_Play("���׹رգ���Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
			{
				XFS_Fail_Play("�������δ����");
			}
			else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
			{
				XFS_Fail_Play("��������Ӧ");
			}
			else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
			{
					char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
					AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
			}
			else
			{
				
			}
	}
}
/*******************************************************************************
* �� �� ��         : Card42_PDeal
* ��������		     : 42��Ԥ���Ѵ�����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card42_PDeal(Card42_PreInfo *Info42p)
{
		Card42_PreInfo      SInfo42pre;         //42��Ԥ������Ϣ�ṹ��
	

		u8  RecvState = 0;
	
		islongthan12 = 0;                  //�ַ����ȳ���12
	  netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ
		
		Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info42p->cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
		HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
		Byte_Split(Card_SendInfo.Card_HexInfo.hexaid, Info42p->areacode, 4);//��2�ֽ�����Ų�ֳ�4�����ֽ��������ת�����ַ�
		HexToChar(Card_SendInfo.Card_CharInfo.straid, Card_SendInfo.Card_HexInfo.hexaid, 4);//��4�����ֽ����������ת���ַ�����
		Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info42p->watersalesamount, 6);//��3�ֽ���ˮ����ֳ�6�����ֽ��������ת�����ַ�
		HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 6);//��6�����ֽ�������ˮ��ת���ַ�����
		
		strcpy(Card_SendInfo.TSendCID,"TSend4442CID%");  //42��Ԥ����CID
		strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //42��Ԥ���ѻ�����
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //42������
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.straid);  //42�������
		strcat(Card_SendInfo.TSendCID,"%");
		strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);//42����ˮ��
		strcat(Card_SendInfo.TSendCID,"%$$");
		strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
		netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
		if(netisbreak==0)     //ͨѶʧ��
		{
			XFS_Fail_Play("42Ԥ����ͨѶʧ�ܣ�������");
			CPU_Reset();
		}
		else                 //ͨѶ�ɹ�
		{
				  Interger_Clear(num,20);
					Char_Find(num,RecieveServerBuf, '%');                                          //��ȡ�ٷֺŵ�λ��
					String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //ȡͷ
					if(strcmp(Card_RecvInfo.head,"SSend4442")==0)                                  //�ж�Ϊ4442��
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
						String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //ȡ������
						String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //ȡ��ֵ��
						String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[4]+1, num[5]);  //ȡ���γ�ֵ��������������
						strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
						/*���������´����ݱ�������*/
						strcpy(Card_SendInfo.TSendCID,"TSend4442Result%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
						strcat(Card_SendInfo.TSendCID,"%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
						strcat(Card_SendInfo.TSendCID,"%");
						strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
						
						Card42_PreReadCID(&SInfo42pre);			  //�ٴζ�4442��
						
						Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo42pre.cardId, 8); //��4�ֽڿ��Ų�ֳ�8�����ֽ��������ת�����ַ�
						HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 8);//��8�����ֽ����鿨��ת���ַ�����
						if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
						{
							StrToHex(Card_SendInfo.Card_HexInfo.hexbuyamount, Card_SendInfo.Card_CharInfo.strbuyamount,3);//����ֵ���ַ���ת��Ϊʮ������
							Card_SendInfo.Card_Flag = Card42_Writesalesamount(Card_SendInfo.Card_HexInfo.hexbuyamount); //����ֵ��д��
							if (Card_SendInfo.Card_Flag == 1)
							{
								strcat(Card_SendInfo.TSendCID,"%Success%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
								if (RecvState == 0)
								{
									XFS_Fail_Play("ͨ��ʧ�ܣ�����ˢ");     //��������������ˢ��
								}
								else if (RecvState == 1)
								{
									if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
									{
										String_Clear(voicesucc, 30);
										strcpy(voicesucc,"���ѳɹ���ֵ");
										strcat(voicesucc, Card_RecvInfo.pricemountstr);
										strcat(voicesucc,"��");
										XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
									}
									else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
									{
										XFS_Fail_Play("����δ��������ˢ");
									}
								}
								
							}
							else
							{
								XFS_Fail_Play("��ֵʧ��,����ˢ"); //������������Ļ��ʾ
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
							}
						}
						else
						{
							strcat(Card_SendInfo.TSendCID,"%Fail%$$");
							strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
							M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
							XFS_Fail_Play("����ˢ");     //��������������ˢ��
						}
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442check")==0)//�ж�Ϊ4442��check
					{
						String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
						String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
						String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //ȡ�û���
						String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //ȡ�û���ַ
						if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
						else islongthan12 = 0;
						String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //ȡ��ˮ����
						String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //ȡ�û�ˮ����ˮ������
						String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //ȡ�û������ѹ�ˮ��
						UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
						UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
						UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
						UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
						UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
						UserInfoDisplay.islongthan12 = islongthan12;
						UserInfoDisplay.WaterInfo.Unit = 1;
						HDMIShowUserDetail(UserInfoDisplay);
						XFS_Only_Play("΢�ŽɷѺ��뽫��ȡ�������²������д������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err0")==0)
					{
						XFS_Fail_Play("δ֪�û�����Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err1")==0)
					{
						XFS_Fail_Play("��΢��֧������ˢ��");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err3")==0)
					{
						XFS_Fail_Play("��Ч������Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"SSend4442err12")==0)
					{
						XFS_Fail_Play("���׹رգ���Ӫҵ������");
					}
					else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
					{
						XFS_Fail_Play("�������δ����");
					}
					else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
					{
						XFS_Fail_Play("��������Ӧ");
					}
					else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
					{
							char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
							AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
					}
					else
					{
						
					}
		}
		
		
}
/*******************************************************************************
* �� �� ��         : Card57_PDeal
* ��������		     : 57��Ԥ���Ѵ�����
* ��    ��         : Info57p:57��Ԥ���ѽṹ��
* ��    ��         : ��
*******************************************************************************/
void Card57_PDeal(Card57_PreInfo * Info57p)
{
	Card57_PreInfo      SInfo57pre;

	u8 k;
	u8  RecvState = 0;
	islongthan12 = 0;                  //�ַ����ȳ���12
	netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ

	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Info57p->cardId, 10); //��5�ֽڿ��Ų�ֳ�10�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//��10�����ֽ����鿨��ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Info57p->watersalesamount, 4);//��2�ֽ���ˮ����ֳ�4�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 4);//��4�����ֽ�������ˮ��ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hextotal, Info57p->total, 4);//��2�ֽ���ˮ������ֳ�4�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.strtotal, Card_SendInfo.Card_HexInfo.hextotal, 4);//��4�����ֽ�������ˮ����ת���ַ�����
	
	strcpy(Card_SendInfo.TSendCID,"TSend5557PCID%");  //57��Ԥ����CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //57��Ԥ���ѻ�����
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //57������
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);  //57����ˮ��
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strtotal);//57����ˮ����
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	if(netisbreak==0)     //ͨѶ������
	{
		XFS_Fail_Play("57Ԥ����ͨѶʧ�ܣ�������");
		CPU_Reset();
	}
	else  //ͨѶ����
	{
				Interger_Clear(num,20);
				Char_Find(num,RecieveServerBuf, '%');                                          //��ȡ�ٷֺŵ�λ��
				String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //ȡͷ
				if(strcmp(Card_RecvInfo.head,"SSend5557P")==0)                                  //�ж�Ϊ57��
				{
					String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
					String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
					String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //ȡ������
					String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[3]+1, num[4]);       //ȡ��ֵ��
					String_Find(Card_RecvInfo.total, RecieveServerBuf, num[4]+1, num[5]);           //ȡ����
					String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[5]+1, num[6]);  //ȡ���γ�ֵ��������������
					/*���������´����ݱ�������*/
					strcpy(Card_SendInfo.TSendCID,"TSend5557PResult%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
					strcat(Card_SendInfo.TSendCID,"%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
					strcat(Card_SendInfo.TSendCID,"%");
					strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
					
					Card57_PreReadCID(&SInfo57pre);                                               //���¶����ٴ���֤�ÿ��Ƿ�����������صĿ���һ��
					
					Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo57pre.cardId, 10); //��5�ֽڿ��Ų�ֳ�10�����ֽ��������ת�����ַ�
					HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//��10�����ֽ����鿨��ת���ַ�����
					
					if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
					{
						strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
						strcpy((char *)Card_SendInfo.Card_CharInfo.strtotal, Card_RecvInfo.total);
						
						StrToHex(SInfo57pre.watersalesamount, Card_SendInfo.Card_CharInfo.strbuyamount,2);//����ֵ���ַ���ת��Ϊʮ������
						StrToHex(SInfo57pre.total, Card_SendInfo.Card_CharInfo.strtotal,2);//����ֵ���ַ���ת��Ϊʮ������
						k = 0;
						while (k<3)
						{
							Card_SendInfo.Card_Flag = Card57_PreWrite(SInfo57pre); //�ѹ�ˮ��д��
							k++;
							if (Card_SendInfo.Card_Flag == 1)
							{
								strcat(Card_SendInfo.TSendCID,"%Success%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
								if (RecvState == 0)
								{
									XFS_Fail_Play("ͨ��ʧ�ܣ�����ˢ");     //��������������ˢ��
								}
								else if (RecvState == 1)
								{
									if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
									{
										String_Clear(voicesucc, 30);
										strcpy(voicesucc,"���ѳɹ���ֵ");
										strcat(voicesucc, Card_RecvInfo.pricemountstr);
										strcat(voicesucc,"��");
										XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 1);
									}
									else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
									{
										XFS_Fail_Play("����δ��������ˢ");
									}
								}
								
								break;
							}
							else
							{						
								if (k == 3)
								{
									XFS_Fail_Play("��ֵʧ��"); //������������Ļ��ʾ
									strcat(Card_SendInfo.TSendCID,"%Fail%$$");
									strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
									M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
									break;
								}
							}
						}
					}
					else
					{
						strcat(Card_SendInfo.TSendCID,"%Fail%$$");
						strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
						M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
						XFS_Fail_Play("����ˢ");     //��������������ˢ��
					}
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Pcheck")==0)//�ж�Ϊ57��ѯ��
				{
					String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
					String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
					String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //ȡ�û���
					String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //ȡ�û���ַ
					if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
					else islongthan12 = 0;
					String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //ȡ��ˮ����
					String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //ȡ�û�ˮ����ˮ������
					String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //ȡ�û������ѹ�ˮ��
					UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
					UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
					UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
					UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
					UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
					UserInfoDisplay.islongthan12 = islongthan12;
					UserInfoDisplay.WaterInfo.Unit = 1;
					HDMIShowUserDetail(UserInfoDisplay);
					XFS_Only_Play("΢�ŽɷѺ��뽫��ȡ�������²������д������");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr0")==0)
				{
					XFS_Fail_Play("δ֪�û�����Ӫҵ������");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr1")==0)
				{
					XFS_Fail_Play("��΢��֧������ˢ��");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr3")==0)
				{
					XFS_Fail_Play("��Ч������Ӫҵ������");
				}
				else if(strcmp(Card_RecvInfo.head,"SSend5557Perr12")==0)
				{
					XFS_Fail_Play("���׹رգ���Ӫҵ������");
				}
				else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
				{
					XFS_Fail_Play("�������δ����");
				}
				else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
				{
					XFS_Fail_Play("��������Ӧ");
				}
				else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
				{
						char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
						AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
				}
				else
				{
					
				}
	}
}
/*******************************************************************************
* �� �� ��         : Card57_LDeal
* ��������		     : 57�����ݴ�����
* ��    ��         : Card57l��57�����ݽṹ��
* ��    ��         : ��
*******************************************************************************/
void Card57_LDeal(Card57_LadderInfo *Card57l)
{
	Card57_LadderInfo   SInfo57Ladder;     //�ڶ��ζ�ȡ
	
  u8 price[20];
	u8 amount[20];

	u8 k;
	u8  RecvState = 0;
	islongthan12 = 0;                  //�ַ����ȳ���12
	netisbreak = 0;                     //�жϵ�ǰ������·�Ƿ�ͨ


	Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, Card57l->cardId, 10); //��5�ֽڿ��Ų�ֳ�10�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//��10�����ֽ����鿨��ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hexamount, Card57l->watersalesamount, 8);//��2�ֽ���ˮ����ֳ�4�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.stramount, Card_SendInfo.Card_HexInfo.hexamount, 8);//��4�����ֽ�������ˮ��ת���ַ�����
	Byte_Split(Card_SendInfo.Card_HexInfo.hextotal, Card57l->total, 6);//��2�ֽ���ˮ������ֳ�4�����ֽ��������ת�����ַ�
	HexToChar(Card_SendInfo.Card_CharInfo.strtotal, Card_SendInfo.Card_HexInfo.hextotal, 6);//��4�����ֽ�������ˮ����ת���ַ�����
	
	strcpy(Card_SendInfo.TSendCID,"TSend5557LCID%");  //57������CID
	strcat(Card_SendInfo.TSendCID, DeviceInfo.TerminalInfo.TerminalId); //57�����ݻ�����
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strcid);  //57������
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.stramount);  //57����ˮ��
	strcat(Card_SendInfo.TSendCID,"%");
	strcat(Card_SendInfo.TSendCID,(const char *)Card_SendInfo.Card_CharInfo.strtotal);//57����ˮ����
	strcat(Card_SendInfo.TSendCID,"%$$");
	strcpy(UploadServerBuf, Card_SendInfo.TSendCID);
	netisbreak = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"%$$", 3,200);
	
	if(netisbreak==0)  //ͨѶʧ�ܣ�����
	{
			//M6311_ReConnect(DeviceInfo.ServerInfo.ServerIP, DeviceInfo.ServerInfo.ServerPort, "����������...");  //M6311���ӷ�����
		  XFS_Fail_Play("57����ͨѶʧ�ܣ�������");
			CPU_Reset();
	}
	else  //ͨѶ����������
	{
			Interger_Clear(num, 20);
			Char_Find(num,RecieveServerBuf, '%');                                          //��ȡ�ٷֺŵ�λ��
			String_Find(Card_RecvInfo.head, RecieveServerBuf, 0, num[0]);                  //ȡͷ
			if(strcmp(Card_RecvInfo.head,"SSend5557L")==0)                                  //�ж�Ϊ57��
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
				String_Find(Card_RecvInfo.orderId, RecieveServerBuf, num[2]+1, num[3]);        //ȡ������
				String_Find(Card_RecvInfo.oneprice, RecieveServerBuf, num[3]+1, num[4]);        //ȡһ��
				String_Find(Card_RecvInfo.twoprice, RecieveServerBuf, num[4]+1, num[5]);        //ȡ����
				String_Find(Card_RecvInfo.threeprice, RecieveServerBuf, num[5]+1, num[6]);      //ȡ����
				String_Find(Card_RecvInfo.oneamount, RecieveServerBuf, num[6]+1, num[7]);       //ȡһ��
				String_Find(Card_RecvInfo.twoamount, RecieveServerBuf, num[7]+1, num[8]);       //ȡ����
				String_Find(Card_RecvInfo.Buyamout, RecieveServerBuf, num[8]+1, num[9]);       //ȡ��ֵ��
				String_Find(Card_RecvInfo.total, RecieveServerBuf, num[9]+1, num[10]);          //ȡ����
				String_Find(Card_RecvInfo.pricemountstr, RecieveServerBuf, num[10]+1, num[11]);  //ȡ���γ�ֵ��������������
				/*���������´����ݱ�������*/
				strcpy(Card_SendInfo.TSendCID,"TSend5557LResult%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.terminalId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.cardId);
				strcat(Card_SendInfo.TSendCID,"%");
				strcat(Card_SendInfo.TSendCID,Card_RecvInfo.orderId);
				
				Card57_LadderReadCID(&SInfo57Ladder);       //��57���ݿ�
				//XFS_HDMI_Play("57���ڶ��ζ�ȡ�ɹ�");
				Byte_Split(Card_SendInfo.Card_HexInfo.hexcid, SInfo57Ladder.cardId, 10); //��5�ֽڿ��Ų�ֳ�10�����ֽ��������ת�����ַ�
				HexToChar(Card_SendInfo.Card_CharInfo.strcid, Card_SendInfo.Card_HexInfo.hexcid, 10);//��10�����ֽ����鿨��ת���ַ�����
				if (strcmp(Card_RecvInfo.cardId, (const char *)Card_SendInfo.Card_CharInfo.strcid) == 0)
				{
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneprice, Card_RecvInfo.oneprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoprice, Card_RecvInfo.twoprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strthreeprice, Card_RecvInfo.threeprice);
					strcpy((char *)Card_SendInfo.Card_CharInfo.stroneamount, Card_RecvInfo.oneamount);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtwoamount, Card_RecvInfo.twoamount);
					
					strcpy((char *)Card_SendInfo.Card_CharInfo.strbuyamount, Card_RecvInfo.Buyamout);
					strcpy((char *)Card_SendInfo.Card_CharInfo.strtotal, Card_RecvInfo.total);
					
					strcpy((char *)price,(char *)Card_SendInfo.Card_CharInfo.stroneprice);
					strcat((char *)price,(char *)Card_SendInfo.Card_CharInfo.strtwoprice);
					strcat((char *)price,(char *)Card_SendInfo.Card_CharInfo.strthreeprice);
					
					strcpy((char *)amount,(char *)Card_SendInfo.Card_CharInfo.stroneamount);
					strcat((char *)amount,(char *)Card_SendInfo.Card_CharInfo.strtwoamount);
					
					StrToHex(SInfo57Ladder.watersalesamount,Card_SendInfo.Card_CharInfo.strbuyamount,4);//����ֵ���ַ���ת��Ϊʮ������
					StrToHex(SInfo57Ladder.total,Card_SendInfo.Card_CharInfo.strtotal,3);//����ֵ�����ַ���ת��Ϊʮ������
					StrToHex(SInfo57Ladder.ladderprice,price,6);
					StrToHex(SInfo57Ladder.ladderamount,amount,3);
					
					k=0;
					while (k<3)
					{
						Card_SendInfo.Card_Flag = Card57_LadderWriteCID(SInfo57Ladder); //�ѹ�ˮ��д��
						k++;
						if (Card_SendInfo.Card_Flag == 1)
						{
							strcat(Card_SendInfo.TSendCID,"%Success%$$");
							strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
							RecvState = M6311_UploadData(RecieveServerBuf,UploadServerBuf,"update", 1,200);
							if (RecvState == 0)
							{
								XFS_Fail_Play("ͨ��ʧ�ܣ�����ˢ");     //��������������ˢ��
							}
							else if (RecvState == 1)
							{
								if (strcmp(RecieveServerBuf, "updateorderstateOK2")==0)
								{
									String_Clear(voicesucc,30);
									strcpy(voicesucc,"���ѳɹ���ֵ");
									strcat(voicesucc, Card_RecvInfo.pricemountstr);
									strcat(voicesucc,"Ԫ");
									XFS_Success_Play(voicesucc, Card_RecvInfo.pricemountstr, 0);
								}
								else if (strcmp(RecieveServerBuf, "updateorderstateOK1")==0)
								{
									XFS_Fail_Play("����δ��������ˢ");
								}
							}
							break;
						}
						else
						{
							if(k==3)
							{
								XFS_Fail_Play("��ֵʧ��"); //������������Ļ��ʾ
								strcat(Card_SendInfo.TSendCID,"%Fail%$$");
								strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
								M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
								break;
							}
						}
					}	
				}
				else
				{
					strcat(Card_SendInfo.TSendCID,"%Fail%$$");
					strcpy(UploadServerBuf,Card_SendInfo.TSendCID);
					M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
					XFS_Fail_Play("����ˢ");								
				}
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lcheck")==0)//�ж�Ϊ57��ѯ��
			{
				String_Find(Card_RecvInfo.terminalId, RecieveServerBuf, num[0]+1, num[1]);     //ȡ������
				String_Find(Card_RecvInfo.cardId, RecieveServerBuf, num[1]+1, num[2]);         //ȡ����
				String_Find(Card_RecvInfo.userId, RecieveServerBuf, num[2]+1, num[3]);         //ȡ�û���
				String_Find(Card_RecvInfo.useradress, RecieveServerBuf, num[3]+1, num[4]);     //ȡ�û���ַ
				if (strlen(Card_RecvInfo.useradress) > 24) islongthan12 = 1;
				else islongthan12 = 0;
				String_Find(Card_RecvInfo.userproperties, RecieveServerBuf, num[4]+1, num[5]);  //ȡ��ˮ����
				String_Find(Card_RecvInfo.Totalamount, RecieveServerBuf, num[5]+1, num[6]);     //ȡ�û�ˮ����ˮ������
				String_Find(Card_RecvInfo.LastAllBuyMount, RecieveServerBuf, num[6]+1, num[7]); //ȡ�û������ѹ�ˮ��
				UserInfoDisplay.UserInfo.userId = Card_RecvInfo.userId;
				UserInfoDisplay.UserInfo.userAdress = Card_RecvInfo.useradress;
				UserInfoDisplay.WaterInfo.waterProperties = Card_RecvInfo.userproperties;
				UserInfoDisplay.WaterInfo.totalAmount = Card_RecvInfo.Totalamount;
				UserInfoDisplay.WaterInfo.LastAllBuyMount = Card_RecvInfo.LastAllBuyMount;
				UserInfoDisplay.islongthan12 = islongthan12;
				UserInfoDisplay.WaterInfo.Unit = 0;
				HDMIShowUserDetail(UserInfoDisplay);
				XFS_Only_Play("΢�ŽɷѺ��뽫��ȡ�������²������д������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr0")==0)
			{
				XFS_Fail_Play("δ֪�û�����Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr1")==0)
			{
				XFS_Fail_Play("��΢��֧������ˢ��");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr3")==0)
			{
				XFS_Fail_Play("��Ч������Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"SSend5557Lerr12")==0)
			{
				XFS_Fail_Play("���׹رգ���Ӫҵ������");
			}
			else if(strcmp(Card_RecvInfo.head,"WebserviceErro")==0)
			{
				XFS_Fail_Play("�������δ����");
			}
			else if(strcmp(Card_RecvInfo.head,"WebNoAnswer")==0)
			{
				XFS_Fail_Play("��������Ӧ");
			}
			else if(strcmp(Card_RecvInfo.head,"commanderr")==0)
			{
					char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
					AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
			}
			else
			{
				
			}
	}
	
}
/*******************************************************************************
* �� �� ��         : CacheOrder_update
* ��������		     : �����˵����� 
* ��    ��         : 
* ��    ��         : ��
*******************************************************************************/
void CacheOrder_update(void)	
{
	char chead[10];//�����е�ͷ
	char clear[5]={0xFF,0xFF,0xFF,0xFF,0xFF};//���ڽ������е�ͷ���
	char crecieve[100];
	
	
	u8  i;
	String_Clear(chead,10);
	String_Clear(UploadServerBuf,100);
	Interger_Clear(num,20);
	
	AT24C02_Read(100,UploadServerBuf,100);                   //��ȡδ���³ɹ��Ķ�������ָ��	
  Char_Find(num,UploadServerBuf, '%');                     //��ȡ�ٷֺŵ�λ��	
	String_Find(chead, UploadServerBuf, 0, num[0]);          //��ȡͷ
	
	if((strcmp(chead,"TSend")==0) &&(strcmp(UploadServerBuf,"$$")==0))
	{
		M6311_SendData(UploadServerBuf, strlen(UploadServerBuf)); //�ϴ����ݵ�������
		for (i=0; i<20; i++)  //�ȴ���������
		{
			if (usart2_rcv_len > 0) 
			{
				delay_ms(341);
				if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"update"))
				{
					break;
				}
			}
			delay_ms(600);
		}
	  if (i < 20)
		{
			String_Clear(RecieveServerBuf,100);
			M6311_USART_GetRcvData(RecieveServerBuf, usart2_rcv_len); //���շ������������½��
			if (strcmp(crecieve, "updateorderstateOK2")==0)
			{
				AT24C02_Write(100,clear,5);	//��ջ����е�ͷ
			}
			else
			{
				CPU_Reset();
			}
		}
	}
	else
	{
		AT24C02_Write(100,clear,5);	
	}
}	

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
