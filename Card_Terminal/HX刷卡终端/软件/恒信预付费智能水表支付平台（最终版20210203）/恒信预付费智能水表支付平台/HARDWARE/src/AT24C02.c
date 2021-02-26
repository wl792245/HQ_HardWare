/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��AT24C02.c
 * ��Ŀ���� ����������24C02�ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#include "AT24C02.h"
#include "IIC_24C02.h"
#include "systick.h"
#include "String_Get.h"

#define AT24C01  127
#define AT24C02  255
#define AT24C04  511
#define AT24C08  1023
#define AT24C16  2047
#define AT24C32  4095
#define AT24C64  8191
#define AT24C128 16383
#define AT24C256 32767

#define EE_TYPE  AT24C02

DeviceInit DeviceInfo;
/*******************************************************************************
* �� �� ��         : AT24C02_Init
* ��������		     : AT24C02��ʼ��   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void AT24C02_Init(void)
{
	IIC_INIT();
}
/*******************************************************************************
* �� �� ��         : AT24C02_ReadOneByte
* ��������		     : 24c02��һ���ֽڵ�ַ  ����	   
* ��    ��         : addr
* ��    ��         : ����ֵtemp
*******************************************************************************/
static u8 AT24C02_ReadOneByte(u16 addr)
{
	u8 temp=0;

	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
		IIC_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0); //  0   ���� NACK
	IIC_NAck();
	IIC_Stop();
	return temp;	
}

/*******************************************************************************
* �� �� ��         : AT24C02_WriteOneByte
* ��������		     : 24c02дһ���ֽڵ�ַ  ����	   
* ��    ��         : addr  dt
* ��    ��         : 0��д�ɹ���1:дʧ��
*******************************************************************************/
static u8 AT24C02_WriteOneByte(u16 addr,u8 dt)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   IIC_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}
	while(IIC_Wait_Ack())
	{
		return 1;
	}
		
	IIC_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	while(IIC_Wait_Ack())
	{
		return 1;
	}
	IIC_Send_Byte(dt);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_ms(10);
	return 0;
}

/*******************************************************************************
* �� �� ��         : AT24C02_Read
* ��������		     : ��AT24C02�����ָ����ַ��ʼ����ָ������������
* ��    ��         : ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255	  
                     pBuffer  :���������׵�ַ
                     NumToRead:Ҫ�������ݵĸ���
* ��    ��         : ��
*******************************************************************************/
void AT24C02_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24C02_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}
/*******************************************************************************
* �� �� ��         : AT24C02_Write
* ��������		     : ��AT24C02�����ָ����ַ��ʼд��ָ������������
* ��    ��         : WriteAddr:��ʼд��ĵ�ַ ��24c02Ϊ0~255
                     pBuffer  :���������׵�ַ
                     NumToRead:Ҫ�������ݵĸ���
* ��    ��         : 0:д�ɹ���1��дʧ��
*******************************************************************************/
u8 AT24C02_Write(u16 WriteAddr,char *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		while(AT24C02_WriteOneByte(WriteAddr,*pBuffer))
		{
			return 1;
		}
		WriteAddr++;
		pBuffer++;
	}
	return 0;
}

/*******************************************************************************
* �� �� ��         : AT24C02IPTid_Init
* ��������		     : ��AT24C02�����ȡ�豸��Ϣ�ͷ�������Ϣ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void AT24C02IPTid_Init(void)
{
	u32 num[20] = {0};                                                                       //���ҷָ����ĸ���
  char TerminalIDServerIP[251];	                                                           //�����ϵͳ24c02�Ļ����źͷ�����ip
	AT24C02_Read(5,TerminalIDServerIP,250);
	Char_Find(num,TerminalIDServerIP, '%');                                                  //��ȡ�ٷֺŵ�λ��
	String_Find(DeviceInfo.TerminalInfo.TerminalId, TerminalIDServerIP, 0, num[0]);          //��ȡ�豸������
	String_Find(DeviceInfo.TerminalInfo.ICCID, TerminalIDServerIP, num[0]+1, num[1]);        //��ȡ�豸ICCID
	String_Find(DeviceInfo.TerminalInfo.ScreenV, TerminalIDServerIP, num[1]+1, num[2]);      //��ȡҺ�����汾��
	String_Find(DeviceInfo.TerminalInfo.TerminalV, TerminalIDServerIP, num[2]+1, num[3]);    //��ȡ�豸�汾��
	String_Find(DeviceInfo.ServerInfo.ServerIP, TerminalIDServerIP, num[3]+1, num[4]);       //��ȡ������IP��
	String_Find(DeviceInfo.ServerInfo.ServerPort, TerminalIDServerIP, num[4]+1, num[5]);     //��ȡ�������˿ں�
	String_Find(DeviceInfo.TerminalInfo.QRcode, TerminalIDServerIP, num[5]+1, num[6]);       //��ȡ�豸��ά��
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
