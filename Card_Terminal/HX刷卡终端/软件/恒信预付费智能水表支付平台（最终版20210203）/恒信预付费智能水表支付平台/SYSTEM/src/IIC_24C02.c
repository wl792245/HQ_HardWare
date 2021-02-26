/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��IIC_24C02.h
 * ��Ŀ���� ��IIC�ӿ����     
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#include "IIC_24C02.h"
#include "systick.h"

/*******************************************************************************
* �� �� ��         : IIC_INIT
* ��������		     : IIC�ӿڳ�ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=IIC_SCL|IIC_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_IIC,&GPIO_InitStructure);
	IIC_SCL_H;
	IIC_SDA_H;
}
/*******************************************************************************
* �� �� ��         : IIC_SDA_OUT
* ��������		     : SDA�������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_SDA_OUT()
{
   GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_IIC,&GPIO_InitStructure);
}
/*******************************************************************************
* �� �� ��         : IIC_SDA_IN
* ��������		     : SDA��������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIO_IIC,&GPIO_InitStructure);
}
/*******************************************************************************
* �� �� ��         : IIC_Start
* ��������		     : ������ʼ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Start(void)
{
  IIC_SDA_OUT();
	IIC_SDA_H;
	IIC_SCL_H;
	delay_us(5);
	IIC_SDA_L;
	delay_us(6);
	IIC_SCL_L;
}
/*******************************************************************************
* �� �� ��         : IIC_Stop
* ��������		     : ����ֹͣ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Stop(void)
{
   IIC_SDA_OUT();
   IIC_SCL_L;
   IIC_SDA_L;
   IIC_SCL_H;
   delay_us(6);
   IIC_SDA_H;
   delay_us(6);
}
/*******************************************************************************
* �� �� ��         : IIC_Ack
* ��������		     : ��������Ӧ���ź�ACK
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_Ack(void)
{
   IIC_SCL_L;
   IIC_SDA_OUT();
   IIC_SDA_L;
   delay_us(2);
   IIC_SCL_H;
   delay_us(5);
   IIC_SCL_L;
}
/*******************************************************************************
* �� �� ��         : IIC_NAck
* ��������		     : ����������Ӧ���ź�NACK
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void IIC_NAck(void)
{
   IIC_SCL_L;
   IIC_SDA_OUT();
   IIC_SDA_H;
   delay_us(2);
   IIC_SCL_H;
   delay_us(5);
   IIC_SCL_L;
}
/*******************************************************************************
* �� �� ��         : IIC_Wait_Ack
* ��������		     : �ȴ��ӻ�Ӧ���ź�
* ��    ��         : ��
* ��    ��         : ����ֵ��1 ����Ӧ��ʧ�� 0 ����Ӧ��ɹ�
*******************************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 tempTime=0;
	IIC_SDA_IN();
	IIC_SDA_H;
	delay_us(1);
	IIC_SCL_H;
	delay_us(1);

	while(GPIO_ReadInputDataBit(GPIO_IIC,IIC_SDA))
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_Stop();
			return 1;
		}	 
	}

	IIC_SCL_L;
	return 0;
}
/*******************************************************************************
* �� �� ��         : IIC_Send_Byte
* ��������		     : ����һ���ֽ�
* ��    ��         : u8���͵�����
* ��    ��         : ��
*******************************************************************************/
void IIC_Send_Byte(u8 txd)
{
	u8 i=0;
	IIC_SDA_OUT();
	IIC_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000
			IIC_SDA_H;
		else
			IIC_SDA_L;

		txd<<=1;
		IIC_SCL_H;
		delay_us(2); //��������
		IIC_SCL_L;
		delay_us(2);
	}
}
/*******************************************************************************
* �� �� ��         : IIC_Read_Byte
* ��������		     : ��ȡһ���ֽ�
* ��    ��         : Ӧ���ź�
* ��    ��         : ���ػ�ȡ���ֽ�
*******************************************************************************/
u8 IIC_Read_Byte(u8 ack)
{
	u8 i=0,receive=0;
	IIC_SDA_IN();
	for(i=0;i<8;i++)
	{
	IIC_SCL_L;
	delay_us(2);
	IIC_SCL_H;
	receive<<=1;
	if(GPIO_ReadInputDataBit(GPIO_IIC,IIC_SDA))
	receive++;
	delay_us(1);	
	}
	if(ack==0)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
