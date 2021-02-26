/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：IIC_24Card.c
 * 项目描述 ：IIC通讯接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#include "IIC_Card.h"
#include "systick.h"

/*******************************************************************************
* 函 数 名         : I2C_INIT
* 函数功能		     : I2C初始化，对24卡进行通讯   
* 输    入         : 
* 输    出         : 无
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
* 函 数 名         : I2C_SCL_OUT
* 函数功能		     : SCL输出配置	   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_SDA_OUT
* 函数功能		     : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_SDA_IN
* 函数功能		     : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void I2C_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*******************************************************************************
* 函 数 名         : I2C_SCL_IN
* 函数功能		     : SCL输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void I2C_SCL_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=I2C_SCL;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}
/*******************************************************************************
* 函 数 名         : I2C_Start
* 函数功能		     : 产生起始信号   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_Stop
* 函数功能		     : 产生停止信号   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_Ack
* 函数功能		     : 主机产生应答信号ACK
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_NAck
* 函数功能		     : 主机不产生应答信号NACK
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : I2C_Wait_Ack
* 函数功能		     : 等待从机应答信号
* 输    入         : 无
* 输    出         : 返回值：1 接收应答失败 0 接收应答成功
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
* 函 数 名         : I2C_Send_Byte
* 函数功能		     : 发送一个字节
* 输    入         : u8发送的数据
* 输    出         : 无
*******************************************************************************/
void I2C_Send_Byte(u8 txd)
{
	u8 i=0;
	I2C_SDA_OUT();
	I2C_SCL_L;//拉低时钟开始数据传输
	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000
			I2C_SDA_H;
		else
			I2C_SDA_L;
		txd<<=1;
		I2C_SCL_H;
		delay_us(2); //发送数据
		I2C_SCL_L;
		delay_us(2);
	}
}
/*******************************************************************************
* 函 数 名         : I2C_Read_Byte
* 函数功能		     : 读取一个字节
* 输    入         : 应答信号
* 输    出         : 返回获取的字节
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
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
