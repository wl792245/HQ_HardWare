/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��BSP_Config.c
 * ��Ŀ���� �����߻���ˢ���ն��豸    
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ��  ��   ��V2.0
 * ʱ  ��   ��2021-01-25
**********************************************************************************/
#include "BSP_Config.h"
#include "string.h"

/*******************************************************************************
* �� �� ��         : BSP_Init
* ��������		     : ������ܳ�ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void BSP_Init(void)
{
	RCC_Configuration();               //�����ⲿʱ��
	NVIC_Configuration();              //�ж���Ӧ����
	delay_init();                      //��ʱ��ʼ�� 
  //�豸��ʼ��
  AT24C02_Init();                    //������AT24C02��ʼ��
	PAM8406_Init();                    //����PAM8406��ʼ��
	HDMI_Init(9600);                   //����3��Һ����ͨѶ��ʼ��
  M6312_Init(115200,0,0);	           //����1��M6312ͨѶ��ʼ��
	PC_Init(9600,0,1);                 //����2����USB������ͨѶ��ʼ��
	HXCard_Init(0,2);                  //��������жϳ�ʼ��
	SCCard_Init(0,2);                  //���������жϳ�ʼ��
	//�豸Ĭ���ȹر�         
	PC_Close();
	PAM8406_Close();
	//�豸��ʼ����
	PC_Open();                         //΢���Կ���
	AT24C02IPTid_Init();               //��ʼ����ȡ�����š�������IPPORT
  M6312_Connect(&DeviceInfo, "����������...");  //M6312���ӷ�����,�ź�ǿ�ȼ�⣬ʱ���ȡ
	FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId); //��һ������
	PC_Start();                        //��������
	TIM_Heart_Init(9999,7199,1,0);     //������ʱ��ʱ��1s��600������һ��
	HDMIShowMenuInfo(&DeviceInfo);     //��������ʾ
	HXCardIsInter = 0;                 //ˢ����־λ�������������֮ǰˢ����Ч
	SCCardIsInter = 0;                 //ˢ����־λ�������������֮ǰˢ����Ч
}
/*******************************************************************************
* �� �� ��         : Remote_ChangeIp
* ��������		     : Զ�̸���IP���˿ڡ���ά��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Remote_ChangeIp(void)
{
	unsigned int Remote_Num[10];
	char  QRcode[21];
	Card_Server  Remote_CardChange;
	M6312_RestartFlag = 0;
	if (usart1_rcv_len > 0)
	{
		String_Clear(TerminalIDServerIP, 100);
		M6312_USART_GetRcvData(TerminalIDServerIP, usart1_rcv_len);
		Get_Head(Remote_CardChange.Card_ServerRecHead, TerminalIDServerIP, '%', Remote_Num, 10); //ȡͷ
		if (strcmp(Remote_CardChange.Card_ServerRecHead,"ChangePc")==0) //����IP
		{
			PC_SendData("HelloMPC%$$");  //�������¼�ʱ
			String_Find(QRcode, TerminalIDServerIP, Remote_Num[3]+1, Remote_Num[4]);          //��ȡ��ά��
			if(strlen(QRcode)>10)
			{
				String_Find(DeviceInfo.TerminalInfo.QRcode, QRcode, 0, strlen(QRcode));
				AT24C02_Write(69,DeviceInfo.TerminalInfo.QRcode,21);
				String_Find(DeviceInfo.ServerInfo.ServerIP, TerminalIDServerIP, Remote_Num[0]+1, Remote_Num[1]);         //��ȡ������IP��
				AT24C02_Write(47,DeviceInfo.ServerInfo.ServerIP,15);
				String_Find(DeviceInfo.ServerInfo.ServerPort, TerminalIDServerIP, Remote_Num[1]+1, Remote_Num[2]);        //��ȡ�������˿ں�
				AT24C02_Write(63,DeviceInfo.ServerInfo.ServerPort,5);
				String_Find(DeviceInfo.TerminalInfo.TerminalId, TerminalIDServerIP, Remote_Num[2]+1, Remote_Num[3]);        //��ȡ������
				AT24C02_Write(5,DeviceInfo.TerminalInfo.TerminalId,10);
				HDMIShowInfo("IP��ά����ĳɹ�");
				PC_StartPam("IP��ά����ĳɹ�");
				M6312_RestartFlag = 0x63;
			}
			else
			{
				HDMIShowInfo("��ά�����ʧ��,������");
				PC_StartPam("��ά�����ʧ��,������");
			}
		}
	}
}


/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
