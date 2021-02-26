/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Card24_Config.h
 * ��Ŀ���� ��24���ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#include "Card24_Config.h"
#include "systick.h"

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



/*******************************************************************************
* �� �� ��         : Card24_ReadOneByte
* ��������		     : 24����һ���ֽڵ�ַ  ����	   
* ��    ��         : addr
* ��    ��         : ����ֵtemp
*******************************************************************************/
u8 Card24_ReadOneByte(u16 addr)
{
	u8 temp=0;
	//Card24_PowerOUT;
	Card24_POWER_H;
	I2C_Start();
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}
	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ	//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte(0xA1);
	I2C_Wait_Ack();
	temp=I2C_Read_Byte(0); //  0   ���� NACK
	I2C_NAck();
	I2C_Stop();
	Card24_POWER_L;
	return temp;	
}
/*******************************************************************************
* �� �� ��         : Card24_WriteOneByte
* ��������		     : ��24����дһ���ֽڵ�ַ������	   
* ��    ��         : addr  dt
* ��    ��         : ��
*******************************************************************************/
void Card24_WriteOneByte(u16 addr,u8 dt)
{
	//Card24_PowerOUT;
	Card24_POWER_H;
	I2C_Start();
	if(EE_TYPE>AT24C16)
	{
		I2C_Send_Byte(0xA0);
		I2C_Wait_Ack();
		I2C_Send_Byte(addr>>8);	//�������ݵ�ַ��λ
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//������ַ+���ݵ�ַ
	}
	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//˫�ֽ������ݵ�ַ��λ		
							//���ֽ������ݵ�ַ��λ
	I2C_Wait_Ack();
	I2C_Send_Byte(dt);
	I2C_Wait_Ack();
	I2C_Stop();
	delay_ms(10);
	Card24_POWER_L; 
}
/*******************************************************************************
* �� �� ��         : Card24_Read
* ��������		     : ��AT24C02�����ָ����ַ��ʼ����ָ������������
* ��    ��         : ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255	  
                     pBuffer  :���������׵�ַ
                     NumToRead:Ҫ�������ݵĸ���
* ��    ��         : ��
*******************************************************************************/
void Card24_Read(u16 ReadAddr,char *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=Card24_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}
/*******************************************************************************
* �� �� ��         : Card24_PreReadCID
* ��������		     : ��24Ԥ���ѿ�id+�����+��ˮ����
* ��    ��         : ���ż�����Ź�7���ֽ�
* ��    ��         : ��
*******************************************************************************/
void Card24_PreReadCID(Card24_PreInfo *Card24pre)
{
	u8 i=0;
	for(i=0;i<4;i++)
	{
		Card24pre->cardId[i]=Card24_ReadOneByte(i);        //������
	}
	Card24pre->cflag=Card24_ReadOneByte(4);              //������־λ
	Card24pre->watersalesamount[0]=Card24_ReadOneByte(6);//����ˮλ���֣�
	Card24pre->watersalesamount[1]=Card24_ReadOneByte(7);
	Card24pre->areacode[0]=  Card24_ReadOneByte(0x75);   //��������
	Card24pre->areacode[1]=  Card24_ReadOneByte(0x76);
	Card24pre->areacode[2]=  Card24_ReadOneByte(0x77);
}
/*******************************************************************************
* �� �� ��         : Card24_PreWriteSaleAmount
* ��������		     : д24Ԥ���ѿ�i��ˮ��
* ��    ��         : ��λ�����ص�2���ֽڵ���ˮ��
* ��    ��         : 1 д���ɹ�  0 д��ʧ��
*******************************************************************************/
u8 Card24_PreWriteSaleAmount(u8 salesamount[2],u8 securitybit[2],u8 times )
{
	Card24_WriteOneByte(0x6,salesamount[0]);//д��ˮλ
	if(salesamount[0]!=Card24_ReadOneByte(0x6)) return 0;//д��ʧ��
	Card24_WriteOneByte(0x7,salesamount[1]);
	if(salesamount[1]!=Card24_ReadOneByte(0x7)) return 0;//д��ʧ��

	Card24_WriteOneByte(0x46,securitybit[0]);//����λ��8λ��70
	if(securitybit[0]!=Card24_ReadOneByte(0x46)) return 0;//д��ʧ��

	Card24_WriteOneByte(0x49,securitybit[1]);//����λ��8λ��73
	if(securitybit[1]!=Card24_ReadOneByte(0x49)) return 0;//д��ʧ��

	Card24_WriteOneByte(0x4A,times);//д����
	if(times!=Card24_ReadOneByte(0x4A)) return 0;//д��ʧ��

	return 1;//д���ɹ�
}
/*******************************************************************************
* �� �� ��         : Card24_LadderReadCID
* ��������		     : ��24���ݿ�id������
* ��    ��         : ���ż�����Ź�7���ֽ�
* ��    ��         : ��
*******************************************************************************/
void Card24_LadderReadCID(Card24_LadderInfo *Card24ladder)
{
	u8 i=0;
	u8 address=0x34;//��ˮ���׵�ַ 
	
	
	for(i=0;i<4;i++)
	{
		Card24ladder->cardId[i]=	Card24_ReadOneByte(i);//������
	}
	Card24ladder->cflag=Card24_ReadOneByte(4);//������־λ
	Card24ladder->areacode[0]=  Card24_ReadOneByte(0x1E);//��������
	Card24ladder->areacode[1]=  Card24_ReadOneByte(0x1F);
	Card24ladder->areacode[2]=  Card24_ReadOneByte(0x20);
	for(i=0;i<4;i++)//����ˮ��
	{
		Card24ladder->watersalesamount[i]=Card24_ReadOneByte(address);
		address++;
	}

}
/*******************************************************************************
* �� �� ��         : Card24_LadderWrite
* ��������		     : д24���ݿ�
* ��    ��         : ��
* ��    ��         : 1 д���ɹ�  0 д��ʧ��
*******************************************************************************/
u8 Card24_LadderWrite(u8 watersalesamount[4],u8 ladderprice[8],u8 ladderamount[6] )
{
	u8 i=0;
	u8 address=0x34;//��ˮ���׵�ַ
	u8 address1=0x15;//���׵�ַ
	u8 address2=0x24;//���׵�ַ
	for(i=0;i<4;i++)
	{
		Card24_WriteOneByte(address,watersalesamount[i]);//д��ˮ��
		if(watersalesamount[i]!=Card24_ReadOneByte(address)) return 0;//д��ʧ��

		address++;
	}

	for(i=0;i<8;i++)
	{
		Card24_WriteOneByte(address1,ladderprice[i]);//д���ݼ�
		if(ladderprice[i]!=Card24_ReadOneByte(address1)) return 0;//д��ʧ��
		address1++;
	}

	for(i=0;i<6;i++)
	{
		Card24_WriteOneByte(address2,ladderamount[i]);//д������
		if(ladderamount[i]!=Card24_ReadOneByte(address2)) return 0;//д��ʧ��
		address2++;
	}  

	return 1;//д���ɹ�
}

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
