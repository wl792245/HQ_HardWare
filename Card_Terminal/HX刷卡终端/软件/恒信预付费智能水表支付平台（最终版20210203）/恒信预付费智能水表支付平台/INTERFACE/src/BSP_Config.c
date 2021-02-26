/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��BSP_Config.c
 * ��Ŀ���� ������Ӳ���ӿ����       
 * ����ƽ̨ ������STM32F103C8T6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2020-10-29
**********************************************************************************/
#include "BSP_Config.h"


#define FirstDownLoad 0  //��һ�����أ���Ҫ����IP��PORT
#define ServerIPPORT  "121.196.181.103%03306%"
#define CID           "1906W10012%00000000000000000000%"
/*******************************************************************************
* �� �� ��         : BSP_Init
* ��������		     : ������ܳ�ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void BSP_Init(void)
{
	M6311_Signal = 31;
	RCC_DeInit();			                 //��λʱ��
	SystemInit();	                     //ϵͳʱ�ӳ�ʼ��
	RCC_Configuration();               //�����ⲿʱ��
	NVIC_Configuration();              //�ж���Ӧ����
	delay_init();                      //��ʱ��ʼ��                
	AT24C02_Init();                    //AT24C02��ʼ��                     
	Card57_Init(1,0);	                 //57����ʼ��
	Card_Init(1,0);                    //24.42����ʼ��
  XFS_Init(115200,3,0);              //����-����4��ʼ�� 
	HDMI_Init(9600,3,1);               //Һ����-����5��ʼ��         
  M6311_Init(115200,2,0);            //2G��-��2��ʼ��
	HDMIShowInfo("ϵͳ��ʼ�����");
	#if FirstDownLoad
	while (AT24C02_Write(47,ServerIPPORT,22))
	{
		XFS_Only_Play("������IP����ʧ��");
	}
	AT24C02_Write(5,CID,32);
	#endif
	delay_ms(1000); 
	AT24C02IPTid_Init();	//��ʼ����ȡ�����ŷ�����IP
	M6311_Connect(DeviceInfo.ServerInfo.ServerIP, DeviceInfo.ServerInfo.ServerPort, "����������...");  //M6311���ӷ�����
	M6311_Signal =M6311_SignalQuery(DeviceInfo.TerminalInfo.SignalStrength);
	HDMISendConncetOK();
	M6311_Connecting = 0;
	FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId);
	CacheOrder_update();               //�˵��������
	TIM_Heart_Init(9999,7199,1,0);     //������ʱ��ʱ��1s��600������һ��
	delay_ms(1000);
	HDMIShowMenuInfo(DeviceInfo.TerminalInfo.QRcode);
	//IWDG_Init();                     //�������Ź���8S֮��ιһ�ξ���
	CardIsInter = 0;                 //�ò���ˢ����־������ˢ��
	Card57_Flag = 0;
}

/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************/
