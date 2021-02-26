/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��IIC_24Card.c
 * ��Ŀ���� ��IICͨѶ�ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-6
**********************************************************************************/
#include "IIC_Card.h"
#include "systick.h"

/*******************************************************************************
* �� �� ��         : I2C_INIT
* ��������		     : I2C��ʼ������24������ͨѶ   
* ��    ��         : 
* ��    ��         : ��
*******************************************************************************/
void I2C_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=I2C_SCL|I2C_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_I2C,&GPIO_InitStructure);
	
	I2C_SCL_H;
	I2C_SDA_H;
}
/*******************************************************************************
* �� �� ��         : I2C_SCL_OUT
* ��������		     : SCL�������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_SCL_OUT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SCL;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*******************************************************************************
* �� �� ��         : I2C_SDA_OUT
* ��������		     : SDA�������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_SDA_OUT()
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

/*******************************************************************************
* �� �� ��         : I2C_SDA_IN
* ��������		     : SDA��������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*******************************************************************************
* �� �� ��         : I2C_SCL_IN
* ��������		     : SCL��������	   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_SCL_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SCL;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*******************************************************************************
* �� �� ��         : I2C_Start
* ��������		     : ������ʼ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_Start(void)
{
  I2C_SDA_OUT();
	I2C_SDA_H;
	I2C_SCL_H;
	delay_us(5);
	I2C_SDA_L;
	delay_us(6);
	I2C_SCL_L;
}
/*******************************************************************************
* �� �� ��         : I2C_Stop
* ��������		     : ����ֹͣ�ź�   
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_Stop(void)
{
   I2C_SDA_OUT();
   I2C_SCL_L;
   I2C_SDA_L;
   I2C_SCL_H;
   delay_us(6);
   I2C_SDA_H;
   delay_us(6);
}
/*******************************************************************************
* �� �� ��         : I2C_Ack
* ��������		     : ��������Ӧ���ź�ACK
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_Ack(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_L;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}
/*******************************************************************************
* �� �� ��         : I2C_NAck
* ��������		     : ����������Ӧ���ź�NACK
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void I2C_NAck(void)
{
   I2C_SCL_L;
   I2C_SDA_OUT();
   I2C_SDA_H;
   delay_us(2);
   I2C_SCL_H;
   delay_us(5);
   I2C_SCL_L;
}
/*******************************************************************************
* �� �� ��         : I2C_Wait_Ack
* ��������		     : �ȴ��ӻ�Ӧ���ź�
* ��    ��         : ��
* ��    ��         : ����ֵ��1 ����Ӧ��ʧ�� 0 ����Ӧ��ɹ�
*******************************************************************************/
u8 I2C_Wait_Ack(void)
{
	u8 tempTime=0;
	I2C_SDA_IN();
	I2C_SDA_H;
	delay_us(1);
	I2C_SCL_H;
	delay_us(1);
	while(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA))
	{
		tempTime++;
		if(tempTime>250)
		{
			I2C_Stop();
			return 1;
		}	 
	}
	I2C_SCL_L;
	return 0;
}
/*******************************************************************************
* �� �� ��         : I2C_Send_Byte
* ��������		     : ����һ���ֽ�
* ��    ��         : u8���͵�����
* ��    ��         : ��
*******************************************************************************/
void I2C_Send_Byte(u8 txd)
{
	u8 i=0;
	I2C_SDA_OUT();
	I2C_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000
			I2C_SDA_H;
		else
			I2C_SDA_L;
		txd<<=1;
		I2C_SCL_H;
		delay_us(2); //��������
		I2C_SCL_L;
		delay_us(2);
	}
}
/*******************************************************************************
* �� �� ��         : I2C_Read_Byte
* ��������		     : ��ȡһ���ֽ�
* ��    ��         : Ӧ���ź�
* ��    ��         : ���ػ�ȡ���ֽ�
*******************************************************************************/

u8 I2C_Read_Byte(u8 ack)
{
   u8 i=0,receive=0;
   I2C_SDA_IN();
   for(i=0;i<8;i++)
   {
   	I2C_SCL_L;
		delay_us(2);
		I2C_SCL_H;
		receive<<=1;
		if(GPIO_ReadInputDataBit(GPIO_I2C,I2C_SDA))
		   receive++;
		delay_us(1);	
   }
   if(ack==0)
	   	I2C_NAck();
	 else
			I2C_Ack();
	return receive;
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
