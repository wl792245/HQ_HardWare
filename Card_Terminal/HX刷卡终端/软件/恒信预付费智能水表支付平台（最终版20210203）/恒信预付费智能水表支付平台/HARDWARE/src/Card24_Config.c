/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card24_Config.h
 * 项目描述 ：24卡接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
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
* 函 数 名         : Card24_ReadOneByte
* 函数功能		     : 24卡读一个字节地址  数据	   
* 输    入         : addr
* 输    出         : 返回值temp
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
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}
	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位	//单字节是数据地址低位
	I2C_Wait_Ack();
	I2C_Start();
	I2C_Send_Byte(0xA1);
	I2C_Wait_Ack();
	temp=I2C_Read_Byte(0); //  0   代表 NACK
	I2C_NAck();
	I2C_Stop();
	Card24_POWER_L;
	return temp;	
}
/*******************************************************************************
* 函 数 名         : Card24_WriteOneByte
* 函数功能		     : 向24卡中写一个字节地址的数据	   
* 输    入         : addr  dt
* 输    出         : 无
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
		I2C_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   I2C_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}
	I2C_Wait_Ack();
	I2C_Send_Byte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	I2C_Wait_Ack();
	I2C_Send_Byte(dt);
	I2C_Wait_Ack();
	I2C_Stop();
	delay_ms(10);
	Card24_POWER_L; 
}
/*******************************************************************************
* 函 数 名         : Card24_Read
* 函数功能		     : 在AT24C02里面的指定地址开始读出指定个数的数据
* 输    入         : ReadAddr :开始读出的地址 对24c02为0~255	  
                     pBuffer  :数据数组首地址
                     NumToRead:要读出数据的个数
* 输    出         : 无
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
* 函 数 名         : Card24_PreReadCID
* 函数功能		     : 读24预付费卡id+区域号+售水次数
* 输    入         : 卡号加区域号共7个字节
* 输    出         : 无
*******************************************************************************/
void Card24_PreReadCID(Card24_PreInfo *Card24pre)
{
	u8 i=0;
	for(i=0;i<4;i++)
	{
		Card24pre->cardId[i]=Card24_ReadOneByte(i);        //读卡号
	}
	Card24pre->cflag=Card24_ReadOneByte(4);              //读卡标志位
	Card24pre->watersalesamount[0]=Card24_ReadOneByte(6);//读售水位（吨）
	Card24pre->watersalesamount[1]=Card24_ReadOneByte(7);
	Card24pre->areacode[0]=  Card24_ReadOneByte(0x75);   //读区域码
	Card24pre->areacode[1]=  Card24_ReadOneByte(0x76);
	Card24pre->areacode[2]=  Card24_ReadOneByte(0x77);
}
/*******************************************************************************
* 函 数 名         : Card24_PreWriteSaleAmount
* 函数功能		     : 写24预付费卡i售水量
* 输    入         : 上位机返回的2个字节的售水量
* 输    出         : 1 写卡成功  0 写卡失败
*******************************************************************************/
u8 Card24_PreWriteSaleAmount(u8 salesamount[2],u8 securitybit[2],u8 times )
{
	Card24_WriteOneByte(0x6,salesamount[0]);//写售水位
	if(salesamount[0]!=Card24_ReadOneByte(0x6)) return 0;//写卡失败
	Card24_WriteOneByte(0x7,salesamount[1]);
	if(salesamount[1]!=Card24_ReadOneByte(0x7)) return 0;//写卡失败

	Card24_WriteOneByte(0x46,securitybit[0]);//保密位高8位放70
	if(securitybit[0]!=Card24_ReadOneByte(0x46)) return 0;//写卡失败

	Card24_WriteOneByte(0x49,securitybit[1]);//保密位低8位放73
	if(securitybit[1]!=Card24_ReadOneByte(0x49)) return 0;//写卡失败

	Card24_WriteOneByte(0x4A,times);//写次数
	if(times!=Card24_ReadOneByte(0x4A)) return 0;//写卡失败

	return 1;//写卡成功
}
/*******************************************************************************
* 函 数 名         : Card24_LadderReadCID
* 函数功能		     : 读24阶梯卡id区域码
* 输    入         : 卡号加区域号共7个字节
* 输    出         : 无
*******************************************************************************/
void Card24_LadderReadCID(Card24_LadderInfo *Card24ladder)
{
	u8 i=0;
	u8 address=0x34;//售水额首地址 
	
	
	for(i=0;i<4;i++)
	{
		Card24ladder->cardId[i]=	Card24_ReadOneByte(i);//读卡号
	}
	Card24ladder->cflag=Card24_ReadOneByte(4);//读卡标志位
	Card24ladder->areacode[0]=  Card24_ReadOneByte(0x1E);//读区域码
	Card24ladder->areacode[1]=  Card24_ReadOneByte(0x1F);
	Card24ladder->areacode[2]=  Card24_ReadOneByte(0x20);
	for(i=0;i<4;i++)//读售水额
	{
		Card24ladder->watersalesamount[i]=Card24_ReadOneByte(address);
		address++;
	}

}
/*******************************************************************************
* 函 数 名         : Card24_LadderWrite
* 函数功能		     : 写24阶梯卡
* 输    入         : 无
* 输    出         : 1 写卡成功  0 写卡失败
*******************************************************************************/
u8 Card24_LadderWrite(u8 watersalesamount[4],u8 ladderprice[8],u8 ladderamount[6] )
{
	u8 i=0;
	u8 address=0x34;//售水额首地址
	u8 address1=0x15;//价首地址
	u8 address2=0x24;//量首地址
	for(i=0;i<4;i++)
	{
		Card24_WriteOneByte(address,watersalesamount[i]);//写售水额
		if(watersalesamount[i]!=Card24_ReadOneByte(address)) return 0;//写卡失败

		address++;
	}

	for(i=0;i<8;i++)
	{
		Card24_WriteOneByte(address1,ladderprice[i]);//写阶梯价
		if(ladderprice[i]!=Card24_ReadOneByte(address1)) return 0;//写卡失败
		address1++;
	}

	for(i=0;i<6;i++)
	{
		Card24_WriteOneByte(address2,ladderamount[i]);//写阶梯量
		if(ladderamount[i]!=Card24_ReadOneByte(address2)) return 0;//写卡失败
		address2++;
	}  

	return 1;//写卡成功
}

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
