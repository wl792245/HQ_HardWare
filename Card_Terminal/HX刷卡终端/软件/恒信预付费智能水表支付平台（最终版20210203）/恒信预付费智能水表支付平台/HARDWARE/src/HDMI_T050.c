/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��HDMI_T050.c
 * ��Ŀ���� ��Һ����T050�봮��5ͨѶ���  
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-8
**********************************************************************************/
#include "HDMI_T050.h"
#include "string.h"
#include "String_Get.h"
#include "M6311.h"
//ҳ��ѡ��
#define PAGE0   "page 0"
#define PAGE1   "page 1"
#define PAGE2   "page 2"
#define PAGE3   "page 3"
#define PAGE4   "page 4"
//
/*******************************************************************************
* �� �� ��         : HDMI_Init
* ��������		     : Һ������ʼ��
* ��    ��         : Bound�������ʣ� m_irq���ж����ȼ���s_irq����Ӧ���ȼ�
* ��    ��         : ��
*******************************************************************************/
void HDMI_Init(u32 Bound, u16 m_irq, u16 s_irq)
{
	UART_HDMI_Init(Bound, m_irq, s_irq);
}
/*******************************************************************************
* �� �� ��         : HDMI_CMD_Send
* ��������		     : ָ���
* ��    ��         : cmd��ָ��
* ��    ��         : ��
*******************************************************************************/
void HDMI_CMD_Send(char *cmd)
{
	UART_HDMI_SendS(cmd);
	UART_HDMI_End;
}
/*******************************************************************************
* �� �� ��         : HDMI_ShowInfo
* ��������		     : ��Ļ��Ϣ��ʾ
* ��    ��         : pDest:����ѡ��pControl���ؼ�ѡ�� pString�������ַ�
* ��    ��         : ��
*******************************************************************************/
static void HDMI_ShowInfo(char *pDest, char *pControl, char *pString)
{
	strcpy(pDest,pControl);
	strcat(pDest,pString);
	strcat(pDest," \"");
  HDMI_CMD_Send(pDest);
}
/*******************************************************************************
* �� �� ��         : HDMI_UnitShowInfo
* ��������		     : ��Ļ����λ��Ϣ��ʾ
* ��    ��         : pDest:����ѡ��pControl���ؼ�ѡ�� pString�������ַ�, unit:��λ
* ��    ��         : ��
*******************************************************************************/
static void HDMI_UnitShowInfo(char *pDest, char *pControl, char *pString, int unit)
{
	strcpy(pDest,pControl);
	strcat(pDest,pString);
	if(unit==0)
	{
		strcat(pDest," Ԫ\"");	
	}
	else
	{
	  strcat(pDest," ��\"");
	}
	HDMI_CMD_Send(pDest);
}
/*******************************************************************************
* �� �� ��         : NetSignalDisplay
* ��������		     : �����ź�ǿ����ʾ
* ��    ��         : SignalValue:�����ź�ǿ��ֵ����ʾ��Ӧ��ͼƬ
* ��    ��         : ��
*******************************************************************************/
void NetSignalDisplay(u8 SignalValue) 
{
	if((SignalValue>0)&&(SignalValue<=5))//�����ź�ǿ������Ļ����ʾ�ź���
	{	
	  HDMI_CMD_Send("p1.pic=10");
	}
	else if((SignalValue>5)&&(SignalValue<=11))//�����ź�ǿ������Ļ����ʾ�ź���
	{					
		HDMI_CMD_Send("p1.pic=11");			
	}
	else if((SignalValue>11)&&(SignalValue<=17))//�����ź�ǿ������Ļ����ʾ�ź���
	{					
		HDMI_CMD_Send("p1.pic=12");			
	}
	else if((SignalValue>17)&&(SignalValue<=23))//�����ź�ǿ������Ļ����ʾ�ź���
	{					
		HDMI_CMD_Send("p1.pic=13");				
	}
	else if((SignalValue>23)&&(SignalValue<=31))//�����ź�ǿ������Ļ����ʾ�ź���
	{					
		HDMI_CMD_Send("p1.pic=14");		
	}
	else
	{
    HDMI_CMD_Send("p1.pic=10");	
	}
}
/*******************************************************************************
* �� �� ��         : HDMISendNetInit
* ��������		     : Һ������ʾ�����ʼ����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HDMISendNetInit(void)  
{
	NetSignalDisplay(0);
	HDMI_CMD_Send(PAGE0);
	HDMI_CMD_Send("t0.txt=\"�����ʼ���С�������\"");
} 	
/*******************************************************************************
* �� �� ��         : HDMISendConncetOK
* ��������		     : Һ�������ӳɹ���ʾ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HDMISendConncetOK(void)         
{ 

	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_CMD_Send("t0.txt=\"���ӳɹ�����\"");
	//�ź�ǿ��ת������
	//NetSignalDisplay(0);
}
/*******************************************************************************
* �� �� ��         : HDMIShowRechargeSucess
* ��������		     : Һ������ʾ��ֵ�ɹ�
* ��    ��         : RechargeSucessValue:������uint :��λ 
* ��    ��         : ��
*******************************************************************************/
void HDMIShowRechargeSucess(char *RechargeSucessValue,int Unit)    
{
	char SucessString[50];
	String_Clear(SucessString, 50);
	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_UnitShowInfo(SucessString, "t0.txt=\"���ѳɹ���ֵ ",RechargeSucessValue,Unit);
}
/*******************************************************************************
* �� �� ��         : HDMIShowUserDetail
* ��������		     : Һ������ʾ�û���Ϣ
* ��    ��         : userId���û�ID
										 userAdress���û���ַ
                     userProperties����ˮ����
										 totalAmount���ۼƹ�������
                     LastAllBuyMount�����깺������
										 Unit:��λ ���� Unit=0  ��ʾ Ԫ��  Ԥ����   Unit=1  ��ʾ  ��	
                     islongthan12��
* ��    ��         : ��
*******************************************************************************/
void HDMIShowUserDetail(UserInfoShow UserInfoDisplay)
{
	char SucessString[50]={'\0'};
	HDMI_CMD_Send(PAGE4);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(SucessString, "t4.txt=\"",UserInfoDisplay.UserInfo.userId);
	if(UserInfoDisplay.islongthan12 == 1)
	{
		HDMI_CMD_Send("vis g1,1");
		HDMI_CMD_Send("vis t5,0");
		String_Clear(SucessString,50);
		HDMI_ShowInfo(SucessString, "g1.txt=\"",UserInfoDisplay.UserInfo.userAdress);
	}
	else
	{
		HDMI_CMD_Send("vis g1,0");
		HDMI_CMD_Send("vis t5,1");
		String_Clear(SucessString,50);
		HDMI_ShowInfo(SucessString, "t5.txt=\"",UserInfoDisplay.UserInfo.userAdress);
	}
	String_Clear(SucessString,50);
	HDMI_ShowInfo(SucessString, "t0.txt=\"",UserInfoDisplay.WaterInfo.waterProperties);
	String_Clear(SucessString,50);
	HDMI_UnitShowInfo(SucessString, "t6.txt=\"",UserInfoDisplay.WaterInfo.totalAmount,UserInfoDisplay.WaterInfo.Unit);
  String_Clear(SucessString,50);
	HDMI_UnitShowInfo(SucessString, "t9.txt=\"",UserInfoDisplay.WaterInfo.LastAllBuyMount,1);
	//�ź�ǿ��
}
/*******************************************************************************
* �� �� ��         : HDMIShowFail
* ��������		     : Һ������ʾʧ��ԭ��
* ��    ��         : reason:ԭ��
* ��    ��         : ��
*******************************************************************************/
void HDMIShowInfo(char *reason) 
{
	char reasonstr[50]={'\0'};
	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(reasonstr, "t0.txt=\"",reason);
}	
/*******************************************************************************
* �� �� ��         : HDMIShowWait
* ��������		     : Һ������ʾ�ȴ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HDMIShowWait(void)
{
	HDMI_CMD_Send(PAGE2);
	HDMIShowSignal(M6311_Signal);
}	

/*******************************************************************************
* �� �� ��         : HDMIShowChangeSuccess
* ��������		     : Һ������ʾ���ĳɹ�
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HDMIShowChangeSuccess(void)         
{ 

	HDMI_CMD_Send(PAGE1);
	HDMIShowSignal(M6311_Signal);
	HDMI_CMD_Send("t0.txt=\"���ĳɹ�\"");
}
/*******************************************************************************
* �� �� ��         : HDMIShowServerInfo
* ��������		     : Һ������ʾ��������Ϣ
* ��    ��         : deviceinfo���豸��Ϣ�Լ���������Ϣ
* ��    ��         : ��
*******************************************************************************/
void HDMIShowServerInfo(pDeviceInit deviceinfo)  
{
	char sysMsg[50];
	String_Clear(sysMsg, 50);
	HDMI_CMD_Send(PAGE3);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(sysMsg, "t1.txt=\"",deviceinfo->TerminalInfo.TerminalId);
	HDMI_ShowInfo(sysMsg, "t3.txt=\"",deviceinfo->TerminalInfo.ICCID);
	HDMI_ShowInfo(sysMsg, "t5.txt=\"",deviceinfo->ServerInfo.ServerIP);
	HDMI_ShowInfo(sysMsg, "t7.txt=\"",deviceinfo->ServerInfo.ServerPort);
	HDMI_ShowInfo(sysMsg, "t9.txt=\"",deviceinfo->TerminalInfo.ScreenV);
	HDMI_ShowInfo(sysMsg, "t11.txt=\"",deviceinfo->TerminalInfo.TerminalV);
	HDMI_ShowInfo(sysMsg, "t13.txt=\"",deviceinfo->TerminalInfo.SignalStrength);
}
/*******************************************************************************
* �� �� ��         : HDMIShowMenuInfo
* ��������		     : Һ������ʾ��������Ϣ
* ��    ��         : deviceinfo��ȡ��ά����Ϣ
* ��    ��         : ��
*******************************************************************************/
void HDMIShowMenuInfo(char * QRcode)
{
 	char sysMsg[100];
  String_Clear(sysMsg,100);
	HDMI_CMD_Send(PAGE0);
	HDMIShowSignal(M6311_Signal);
	HDMI_ShowInfo(sysMsg, "qr0.txt=\"http://weixin.qq.com/r/",QRcode);//44��Ԫ�أ���ӦQRcode[44]//https://u.wechat.com/EEAAXRbIIE0YxMhmj9CmUCU//\"http://weixin.qq.com/r/
}
/*******************************************************************************
* �� �� ��         : HDMIShowSignal
* ��������		     : Һ������ʾ�ź�ǿ��
* ��    ��         : SignalValue:�ź�ֵ
* ��    ��         : ��
*******************************************************************************/
void HDMIShowSignal(unsigned char SignalValue)
{
	if((SignalValue>0)&&(SignalValue<=5))//�����ź�ǿ������Ļ����ʾ�ź���
	{
		HDMI_CMD_Send("p1.pic=10");					
	}
	else if((SignalValue>5)&&(SignalValue<=11))//�����ź�ǿ������Ļ����ʾ�ź���
	{
		HDMI_CMD_Send("p1.pic=11");					
	}
	else if((SignalValue>11)&&(SignalValue<=17))//�����ź�ǿ������Ļ����ʾ�ź���
	{
		HDMI_CMD_Send("p1.pic=12");					
	}
	else if((SignalValue>17)&&(SignalValue<=23))//�����ź�ǿ������Ļ����ʾ�ź���
	{
		HDMI_CMD_Send("p1.pic=13");					
	}
	else if((SignalValue>23)&&(SignalValue<=31))//�����ź�ǿ������Ļ����ʾ�ź���
	{
		HDMI_CMD_Send("p1.pic=14");					
	}
	else
	{
		;	
	}
}
/*******************************************************************************
* �� �� ��         : HardWareErr
* ��������		     : Һ������ʾӲ��������ϵ����
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void HardWareErr(void)
{
	HDMI_CMD_Send(PAGE1);
	HDMI_CMD_Send("t0.txt=\"Ӳ������,��ϵ����\"");
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
