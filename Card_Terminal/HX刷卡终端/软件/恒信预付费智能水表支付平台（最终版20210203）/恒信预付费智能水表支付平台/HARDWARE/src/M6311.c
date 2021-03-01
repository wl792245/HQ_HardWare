/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��M6311.c
 * ��Ŀ���� ��M6311ģ�鹦�ܶ���     
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * Ӳ������ ��
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-4
**********************************************************************************/
#include "M6311.h"
#include "HDMI_T050.h"
#include "stm32f10x.h"
#include "systick.h"
#include "string.h"
#include "String_Get.h"
#include "utils.h"
#include "XFS.h"
char M6311ConfigMsg[80];  //IP�Ͷ˿��ַ����
char M6311_RecieveServerBuf[50];//���շ��������صĽ�� 
unsigned char M6311_Signal = 31;      //��ʼĬ���ź�ǿ��
unsigned char M6311_Connecting = 1;   //����������
volatile unsigned char M6311_RestartFlag = 0;  //M6311������־λ

/*******************************************************************************
* �� �� ��         : M6311POWER_Init
* ��������		     : M6311��Դ���ų�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void M6311POWER_Init(void)	 
{
	GPIO_InitTypeDef GPIO_InitStructure;	 

	//SystemInit();//����΢������ϵͳ��ʼ��Ƕ��ʽ����ӿڡ�PLL������ϵͳcoclock������	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);  //���û���ø���apb��apb2����Χʱ�ӡ�
	
	GPIO_InitStructure.GPIO_Pin=M6311_POWER_PIN ;	  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�������  
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //��������50MHz
	GPIO_Init(M6311_POWER_PORT,&GPIO_InitStructure); //����gpio_init�ṹ�е�ָ��������ʼ��gpiox��Χ��
}
/*******************************************************************************
* �� �� ��         : M6311_Init
* ��������		     : 2Gģ���ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void M6311_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	USART_M6311_Init(Bound, m_irq, s_irq);
	M6311POWER_Init();
}
/*******************************************************************************
* �� �� ��         : M6311_ReConnect
* ��������		     : M6311��������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static int M6311_RConnect(void)
{
	char i,j = 0;
	M6311_Connecting = 1;
	M6311_POWER_L;		//��GPRSģ���Դ
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
	M6311_POWER_H;
	for(i=0; i < 5;i++)
	{
		delay_ms(341);
	}
 	//���������������ʾ
	for(i=0; i < 18;i++)    //ÿ9��������������
	{	
		for(j=0;j<30;j++)			//�ȴ��������ݽ���
		{
			if(usart2_rcv_len>0)
			{
				delay_ms(341);
				break;
			}
			delay_ms(341);
		}
		if(j < 30)
		{
			if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)"CTZV"))
			{
				return 1;
			}
			usart2_rcv_len = 0;
		}

	}
	return 0;
}
/*******************************************************************************
* �� �� ��         : M6311_Connect
* ��������		     : 2Gģ������
* ��    ��         : IP:IP��ַ��Port:�˿ں�
* ��    ��         : ��
*******************************************************************************/
void M6311_Connect(const pDeviceInit pServerInfo, char *ShowInfo)
{
  int isStartNet = 1;      //�����Ƿ�ɹ���־λ
  int isConnect = 0;
	HDMIShowInfo(ShowInfo);
	while (isStartNet)
	{
		isConnect = M6311_RConnect();
		if(isConnect)
		{
			//�ϵ��ʼ��
			if(USART_M6311_SendCmd("AT\r\n","OK",842)==0) //����AT,���������������Ƿ��ܺ� DCE ģ������ͨѶ
			{
				continue; 			
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+IPR=115200;&W \r\n","OK",842)==0) //�������û��ȡ DCE �Ĳ����ʡ��������˲����ʺ󣬶�Ӧ�Ĺ����糬��
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGREG?\r\n","+CGREG: 0,1",842)==0)  //  ���� MT ��ʾ����ע��״̬��λ����Ϣ
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK", 862)==0) /*MT ���� PDP �����ļ�����Ϣ��ʱ��ʹ�������������
														PDP �����Ĳ�����ϵͳ�������󣬸��������������ý���������*/
			{
				continue; 
			}
			delay_ms(941);
			if(USART_M6311_SendCmd("AT+CGACT=1,1 \r\n","OK", 941)==0) //0��PDP ������ȥ��,1��PDP �����ļ���
			{
				continue; 
			}
			delay_ms(941);
			//����TCP����
			if(USART_M6311_SendCmd("AT+CMMUX=0\r\n","OK",862)==0) //�����Ƿ����ö�·����
			{
				continue; 
			}
			delay_ms(941); 
			if(USART_M6311_SendCmd("AT+CMMODE=1\r\n","OK",862)==0) /*	1��͸��ģʽ���ڸ�ģʽ�£��������ӳɹ��� 
														UART �����������ģʽ�����յ������ݽ�ֱ����������ڣ�
														�������ݻᷢ��������������ͨ����+++���˳�����ģʽ��
														�˳�����ʹ�� AT+IPCLOSE �ͷ���Դ��*/
			{
				continue; 
			}
			delay_ms(941);
			
			if(USART_M6311_SendCmd("AT+CMTCFG=1,1024\r\n","OK",862)==0) //���� TCPIP ͸��ģʽ
			{
				continue; 
			}
			delay_ms(941); 

			strcpy(M6311ConfigMsg,"AT+IPSTART=\"TCP\",\"");//���� TCP ���ӻ�ע�� UDP �˿ں�
			strcat(M6311ConfigMsg,pServerInfo->ServerInfo.ServerIP);
			strcat(M6311ConfigMsg,"\",");
			strcat(M6311ConfigMsg,pServerInfo->ServerInfo.ServerPort);
			strcat(M6311ConfigMsg,"\r\n");
			if(USART_M6311_SendCmd(M6311ConfigMsg,"CONNECT",941)==0)//30000->3000��20181022��3���㹻
			{
				continue; //������ϣ�ϵͳ��λ����
			}
			M6311_SignalQuery(pServerInfo->TerminalInfo.SignalStrength);
			if (0x63 == M6311_RestartFlag)
			{
				continue;
			}
			Restart_Count = ReStart_Read(DeviceInfo.TerminalInfo.ReStart); //��ȡ��������
			Restart_Count +=1;
			ResStart_Write(DeviceInfo.TerminalInfo.ReStart, Restart_Count);  //д����������
			isStartNet = 0;
			HDMISendConncetOK();
			delay_ms(941);
		}
	}
}
/*******************************************************************************
* �� �� ��         : M6311_SendData
* ��������		     : ͨ��M6311���������������
* ��    ��         : buf:�ַ�����len:����
* ��    ��         : ��
*******************************************************************************/
void M6311_SendData(char * buf,uint32_t len)
{

	String_Clear((char *)usart2_rcv_buf,strlen((const char *)usart2_rcv_buf));
	usart2_rcv_len=0;
	USART_M6311_Write(buf,len);	
	delay_ms(20);	
}
/*******************************************************************************
* �� �� ��         : M6311_UploadData
* ��������		     : ͨ��M6311��������ϴ�����
* ��    ��         : pdest:�����ַ�����psrc:�ϴ��ַ���  len:���ճ���
* ��    ��         : 1:�ɹ����յ����� 0����������ʧ��
*******************************************************************************/
u8 M6311_UploadData(char * pdest,char *psrc, char *txt, int times,int len)
{
		u8 Sendtimes = 0;
	  u8 i;
	  while(Sendtimes < times) //û���ؾ���������3��
		{
			M6311_SendData(psrc, strlen(psrc)); //�ϴ����ݵ�������
			Sendtimes++;  
			for (i=0; i<20; i++)  //�ȴ���������
			{
				if (usart2_rcv_len > 0) 
				{
					delay_ms(341);
					if (NULL != strstr((const char *)usart2_rcv_buf, (const char *)txt))
					{
						String_Clear(pdest, len);
						M6311_USART_GetRcvData(pdest, usart2_rcv_len);
						return 1;
					}
				}
				delay_ms(600);
			}
		}
		return 0;
}

/*******************************************************************************
* �� �� ��         : M6311_USART_GetRcvData
* ��������		     : ������ͨ��M6311��Ƭ�����ڷ�����Ϣ
* ��    ��         : buf����Ƭ�������������飬 len�����ݳ���
* ��    ��         : ��
*******************************************************************************/
void M6311_USART_GetRcvData(char *buf, uint32_t rcv_len)
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
* �� �� ��         : M6311_SignalQuery
* ��������		     : ��ѯ�ź�ǿ�� 
* ��    ��         : Signal���ź�ǿ��ֵ
* ��    ��         : ��
*******************************************************************************/
void M6311_SignalQuery(char *Signal)
{
	u8 i,j,k;
	
	M6311_RestartFlag = 0;
	k = 0;
	while(k<3)
	{
		k++;
		if(USART_M6311_SendCmd("+++","OK",842)==1)//��ѯ�ź�ǿ��
		{
			break;
		}
	}
	if (k >= 3)
	{
		M6311_RestartFlag = 0x63;
	}
	else
	{
		k = 0;
		while (k<3)
		{
			k++;
			if(USART_M6311_SendCmd("AT+CSQ\r\n","CSQ",842)==1)//��ѯ�ź�ǿ��
			{
				break;
			}
		}
		if (k >= 3)
		{
			M6311_RestartFlag = 0x63;  //
		}
		else
		{
			String_Clear(M6311_RecieveServerBuf, 50);
			M6311_USART_GetRcvData(M6311_RecieveServerBuf, usart2_rcv_len);
			i=0;
			while (M6311_RecieveServerBuf[i]!=':') 
			{
				i++; 
			}
			i++;
			while (M6311_RecieveServerBuf[i]!=' ')
			{
				i++; 
			}
			j=0;
			i++;
			while (M6311_RecieveServerBuf[i]!=',') 
			{						
				Signal[j]=M6311_RecieveServerBuf[i];//ȡ�ź�ǿ��
				i++;j++;
			}
			Signal[i]='\0';
			if(USART_M6311_SendCmd("ATO\r\n","OK",842)==1)// �ص���������ģʽ
			{
				StrToDec(&M6311_Signal,Signal);	
			}
			else
			{
				M6311_RestartFlag = 0x63;  //
			}
		}
	}
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
