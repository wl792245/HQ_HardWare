/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：IIC_24C02.h
 * 项目描述 ：IIC接口设计     
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#include "IIC_24C02.h"
#include "systick.h"

/*******************************************************************************
* 函 数 名         : IIC_INIT
* 函数功能		     : IIC接口初始化
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_SDA_OUT
* 函数功能		     : SDA输出配置	   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_SDA_IN
* 函数功能		     : SDA输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIO_IIC,&GPIO_InitStructure);
}
/*******************************************************************************
* 函 数 名         : IIC_Start
* 函数功能		     : 产生起始信号   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_Stop
* 函数功能		     : 产生停止信号   
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_Ack
* 函数功能		     : 主机产生应答信号ACK
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_NAck
* 函数功能		     : 主机不产生应答信号NACK
* 输    入         : 无
* 输    出         : 无
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
* 函 数 名         : IIC_Wait_Ack
* 函数功能		     : 等待从机应答信号
* 输    入         : 无
* 输    出         : 返回值：1 接收应答失败 0 接收应答成功
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
* 函 数 名         : IIC_Send_Byte
* 函数功能		     : 发送一个字节
* 输    入         : u8发送的数据
* 输    出         : 无
*******************************************************************************/
void IIC_Send_Byte(u8 txd)
{
	u8 i=0;
	IIC_SDA_OUT();
	IIC_SCL_L;//拉低时钟开始数据传输
	for(i=0;i<8;i++)
	{
		if((txd&0x80)>0) //0x80  1000 0000
			IIC_SDA_H;
		else
			IIC_SDA_L;

		txd<<=1;
		IIC_SCL_H;
		delay_us(2); //发送数据
		IIC_SCL_L;
		delay_us(2);
	}
}
/*******************************************************************************
* 函 数 名         : IIC_Read_Byte
* 函数功能		     : 读取一个字节
* 输    入         : 应答信号
* 输    出         : 返回获取的字节
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
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
