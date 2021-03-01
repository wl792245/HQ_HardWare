/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��M6312.c
 * ��Ŀ���� ��M6312ģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * Ӳ������ ��M6312(RX)--PA9 M6312(TX)--PA10 M6312(POWER)--PA15
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#include "M6312.h"
#include "HDMI_T050.h"
#include "stm32f10x.h"
#include "systick.h"
#include "string.h"
#include "PC_Config.h"
#include "String_Config.h"
#include "utils.h"
//#include "XFS.h"
char M6312ConfigMsg[80];  //IP�Ͷ˿��ַ����
char M6312_RecieveServerBuf[50];//���շ��������صĽ�� 
 unsigned char M6312_Signal = 10;      //��ʼĬ���ź�ǿ��
volatile unsigned char M6312_RestartFlag = 0;      //M6312������־λ
 unsigned char M6312_Connecting = 1;   //����������
/*******************************************************************************
* �� �� ��         : M6312POWER_Init
* ��������		     : M6312��Դ���ų�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void M6312POWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//����΢������ϵͳ��ʼ��Ƕ��ʽ����ӿڡ�PLL������ϵͳcoclock������	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);  //���û���ø���apb��apb2����Χʱ�ӡ�
	//�������PB3��PB4��PA15����ͨIO��PA13&14����SWD���ԣ�
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin=M6312_POWER_PIN;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //��������50MHz
	GPIO_Init(M6312_POWER_PORT,&GPIO_InitStructure); //����gpio_init�ṹ�е�ָ��������ʼ��gpiox��Χ��
  M6312_POWER_L;		//��GPRSģ���Դ
}
/*******************************************************************************
* �� �� ��         : M6312_Init
* ��������		     : 2Gģ���ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void M6312_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	M6312POWER_Init();
	USART_M6312_Init(Bound, m_irq, s_irq);
}
/*******************************************************************************
* �� �� ��         : M6312_ReConnect
* ��������		     : M6312��������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static int M6312_RConnect(void)
{
	char i,j = 0;
	M6312_Connecting = 1;
	M6312_POWER_L;		//��GPRSģ���Դ
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
	M6312_POWER_H;
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
 	//���������������ʾ
	for(i=0; i < 18;i++)    //ÿ9��������������
	{	
		for(j=0;j<30;j++)			//�ȴ��������ݽ���
		{
			if(usart1_rcv_len>0)
			{
				delay_ms(341);
				break;
			}
			delay_ms(341);
		}
		if(j < 30)
		{
			if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)"CTZV"))
			{
				return 1;
			}
			usart1_rcv_len = 0;
		}

	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : M6312_Connect
* ��������		     : 2Gģ������
* ��    ��         : IP:IP��ַ��Port:�˿ں�
* ��    ��         : ��
*******************************************************************************/
void M6312_Connect(pDeviceInit pServerInfo,char *ShowInfo)
{
  int isStartNet = 1;      //�����Ƿ�ɹ���־λ
  int isConnect = 0;

  M6312_Signal = 0;
	HDMIShowInfo(ShowInfo);

	while (isStartNet)
	{
		isConnect = M6312_RConnect();
		delay_ms(941);
		if(isConnect)
		{
			//�ϵ��ʼ��
			if(USART_M6312_SendCmd("AT\r\n","OK",842)==0) //����AT,���������������Ƿ��ܺ� DCE ģ������ͨѶ
			{
				continue; 			
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CPIN?\r\n","READY",842)==0) //���� READY����ʾ SIM ������ʶ������K
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGREG?\r\n","+CGREG: 0,1",842)==0) //ȷ�����������ɹ�,1��5��ʾע��ɹ�
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK", 862)==0) //���� GPRS �� APN��������Ϊ SIM ��ǩԼ��Ӧ�� APN��
			{
				continue;
			}
			delay_ms(941);
			if(USART_M6312_SendCmd("AT+CGACT=1,1\r\n","OK", 941)==0) //���� PDP������ GPRS ���磩
			{
				continue;
			}
			delay_ms(941);
			//����TCP����
			if(USART_M6312_SendCmd("AT+CMMUX=0\r\n","OK",862)==0) //�����Ƿ����ö�·����
			{
				continue;
			}
			delay_ms(941); 
			if(USART_M6312_SendCmd("AT+CMHEAD=1\r\n","OK",862)==0) /*	1��͸��ģʽ���ڸ�ģʽ�£��������ӳɹ��� 
														UART �����������ģʽ�����յ������ݽ�ֱ����������ڣ�
														�������ݻᷢ��������������ͨ����+++���˳�����ģʽ��
														�˳�����ʹ�� AT+IPCLOSE �ͷ���Դ��*/
			{
				continue;
			}
			delay_ms(941);
			
			if(USART_M6312_SendCmd("AT+CMSHOWRA=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941); 
			
			if(USART_M6312_SendCmd("AT+CMSHOWLA=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941); 
			
			if(USART_M6312_SendCmd("AT+CMSHOWPT=1\r\n","OK",862)==0) 
			{
				continue;
			}
			delay_ms(941);
			//����͸��
			if(USART_M6312_SendCmd("AT+CMMODE=1\r\n","OK",862)==0) //���� TCPIP ͸��ģʽ
			{
				continue;
			}
			delay_ms(941);
			strcpy(M6312ConfigMsg,"AT+IPSTART=\"TCP\",\"");//���� TCP ���ӻ�ע�� UDP �˿ں�
			strcat(M6312ConfigMsg,pServerInfo->ServerInfo.ServerIP);
			strcat(M6312ConfigMsg,"\",");
			strcat(M6312ConfigMsg,pServerInfo->ServerInfo.ServerPort);
			strcat(M6312ConfigMsg,"\r\n");
			if(USART_M6312_SendCmd(M6312ConfigMsg,"CONNECT",941)==0)//30000->3000��20181022��3���㹻
			{
				continue;//������ϣ�ϵͳ��λ����
			}
			//���ӳɹ�
			//HDMIShowInfo("���ӳɹ�");
			M6312_SignalQuery(pServerInfo->TerminalInfo.SignalStrength);  //�ź�ǿ�ȼ��
			if (0x63 == M6312_RestartFlag)
			{
				continue;
			}
			Restart_Count = ReStart_Read(DeviceInfo.TerminalInfo.ReStart); //��ȡ��������
			Restart_Count +=1;
			ResStart_Write(DeviceInfo.TerminalInfo.ReStart, Restart_Count);  //д����������
			isStartNet = 0;
			delay_ms(941);
		}
	}
}
/*******************************************************************************
* �� �� ��         : M6312_SendData
* ��������		     : ͨ��M6312���������������
* ��    ��         : buf:�ַ�����len:����
* ��    ��         : ��
*******************************************************************************/
void M6312_SendData(char * buf,uint32_t len)
{

	String_Clear((char *)usart1_rcv_buf,strlen((const char *)usart1_rcv_buf));
	usart1_rcv_len=0;
	USART_M6312_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* �� �� ��         : M6312_UploadData
* ��������		     : ͨ��M6312��������ϴ�����
* ��    ��         : pdest:�����ַ�����psrc:�ϴ��ַ���  txt���ַ���times������ len:���ճ���
* ��    ��         : 1:�ɹ����յ����� 0����������ʧ��
*******************************************************************************/
u8 M6312_UploadData(char * pdest,char *psrc, char *txt, int times,int len)
{
		u8 Sendtimes = 0;
	  u8 netisbreak = 0;
	  u8 i;
	  while(Sendtimes < times) //û���ؾ���������3��
		{
			M6312_SendData(psrc, strlen(psrc)); //�ϴ����ݵ�������
			Sendtimes++;  
			for (i=0; i<10; i++)  //�ȴ���������
			{
				if (usart1_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart1_rcv_buf, (const char *)txt))
					{
						break;
					}
				}
				delay_ms(500);
			}
			if (i < 10)
			{
				netisbreak = 1;
				break;
			}
			
		}
		if (netisbreak == 1)
		{
				String_Clear(pdest, len);
				M6312_USART_GetRcvData(pdest, usart1_rcv_len);
				return 1;
		}
		else
		{
			//ͨѶʧ��
			PC_SendData("CardOff%$$");
			HDMIShowInfo("��������Ӧ");
			delay_ms(10);
			PC_StartPam("��������Ӧ");
			delay_ms(100);
			return 0;
		}
		
}
/*******************************************************************************
* �� �� ��         : M6312_USART_GetRcvData
* ��������		     : ������ͨ��M6312��Ƭ�����ڷ�����Ϣ
* ��    ��         : buf����Ƭ�������������飬 len�����ݳ���
* ��    ��         : ��
*******************************************************************************/
void M6312_USART_GetRcvData(char *buf, uint32_t rcv_len)
{
	u32 i;
	for (i=0; i<rcv_len; i++)
	{
		buf[i] = usart1_rcv_buf[i];
	}
	buf[i] = '\0';
	String_Clear((char *)usart1_rcv_buf,strlen((const char *)usart1_rcv_buf));
	usart1_rcv_len = 0;
}
/*******************************************************************************
* �� �� ��         : M6312_SignalQuery
* ��������		     : ��ѯ�ź�ǿ��
* ��    ��         : Signal:�ź�ǿ��ֵ
* ��    ��         : ��
*******************************************************************************/
void M6312_SignalQuery(char *Signal)
{
	u8 i,j,k;


	k = 0;
	M6312_RestartFlag = 0;
	while(k<3)
	{
		k++;
		USART_M6312_Write("+++", strlen("+++"));
		delay_ms(999);
		if(USART_M6312_SendCmd("AT+CSQ\r\n","CSQ",1000)==1)//��ѯ�ź�ǿ��
		{
			break;
		}
	}
	if(k >= 3)
	{
		M6312_RestartFlag = 0x63;
	}
	else
	{
		String_Clear(M6312_RecieveServerBuf, 50);
		M6312_USART_GetRcvData(M6312_RecieveServerBuf, usart1_rcv_len);
		i=0;
		while (M6312_RecieveServerBuf[i]!=':') 
		{
			i++; 
		}
		i++;
		while (M6312_RecieveServerBuf[i]!=' ')
		{
			i++; 
		}
		j=0;
		i++;
		while (M6312_RecieveServerBuf[i]!=',') 
		{						
			Signal[j]=M6312_RecieveServerBuf[i];//ȡ�ź�ǿ��
			i++;j++;
		}
		Signal[i]='\0';
		//ʱ���ѯ
		if(1 == M6312_TimeQuery(&DeviceInfo.DataInfo))
		{
			M6312_RestartFlag = 0x63;
		}
		if(USART_M6312_SendCmd("ATO\r\n","OK",1000)==1)// �ص���������ģʽ
		{
			StrToDec(&M6312_Signal,Signal);
		}
		else
		{
			M6312_RestartFlag = 0x63;
		}
	}
}
/*******************************************************************************
* �� �� ��         : M6312_TimeQuery
* ��������		     : ʱ����
* ��    ��         : TimeInit��ʱ��ָ��ṹ��
* ��    ��         : 1����ȡʧ�ܣ� 0����ȡ�ɹ�
*******************************************************************************/
unsigned char M6312_TimeQuery(PDataInit TimeInit)
{
	u8 i,j,k;
	k = 0;
	M6312_RestartFlag = 0;
	while(k<3)
	{
		k++;
		if(USART_M6312_SendCmd("AT+CCLK?\r\n","OK",1000)==1)//ʱ����
		{
			break;
		}
	}
	if (k >= 3)
	{
		return 1;
	}
	else
	{
		String_Clear(M6312_RecieveServerBuf, 50);
		M6312_USART_GetRcvData(M6312_RecieveServerBuf, usart1_rcv_len);
		i=0;
		while (M6312_RecieveServerBuf[i]!='"')
		{
			i++; 
		}
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]=='/') break;
			TimeInit->Year[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Year[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]=='/') break;
			TimeInit->Mounth[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Mounth[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==',') break;
			TimeInit->Day[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Day[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==':') break;
			TimeInit->Hour[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Hour[j]='\0';
		j=0;
		for(i++;i<strlen(M6312_RecieveServerBuf);i++)//
		{
			if(M6312_RecieveServerBuf[i]==':') break;
			TimeInit->Minute[j]=M6312_RecieveServerBuf[i];
			j++;
		}
		TimeInit->Minute[j]='\0';
		return 0;
	}
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
