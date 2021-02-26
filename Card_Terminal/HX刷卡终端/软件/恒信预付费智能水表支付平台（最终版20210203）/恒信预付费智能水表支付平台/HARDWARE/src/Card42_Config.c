/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card42_Config.c
 * 项目描述 ：4442卡通讯接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Card42_Config.h"
#include "systick.h"

#define SLE4442ID1   0xA2     
#define SLE4442ID2   0x13
#define SLE4442ID3   0x10
#define SLE4442ID4   0x91


/*******************************************************************************
* 函 数 名         : delay15us
* 函数功能		     : 延时15微秒 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void delay15us(void)
{
	delay_us(15) ;
}

/*******************************************************************************
* 函 数 名         : Card42_PowerOn
* 函数功能		     : 打开4442卡电源  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card42_PowerOn(void)
{
	u8 i;
	Card42_POWER_H;
	for(  i=0;i<10;i++) delay15us();
}
/*******************************************************************************
* 函 数 名         : Card42_PowerOff
* 函数功能		     : 关闭4442卡电源  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card42_PowerOff(void)
{
	Card42_POWER_L;
}
/*******************************************************************************
* 函 数 名         : Card42_Card42_WriteByte
* 函数功能		     : 给4442卡写字节,低字节输出 
* 输    入         : 数据输入
* 输    出         : 无
*******************************************************************************/
static void Card42_WriteByte(u8 Byte)
{
	u8 i=0;
	Card42_SDA_OUT;
	for(i=0;i<8;i++) 
	{ 
		Card42_SCL_L;		//LOW SCL
		if((Byte&0x01)==0)
		{
			Card42_SDA_L;
			//delay_us(5);
		}
		else
		{
			Card42_SDA_H;		
		}
		delay_us(10);
		Card42_SCL_H;			//HIGH SCL
		delay_us(10);
		Card42_SCL_L;
		Byte=Byte>>1;
	}
}
/*******************************************************************************
* 函 数 名         : Card42_Readtemp
* 函数功能		     : 读4442卡数据 
* 输    入         : addr：地址 readdata：字符串
* 输    出         : 字节
*******************************************************************************/
u8 Card42_ReadString(u8 addr,u8 *Stringdata)
{
	u8 i=0;
	Card42_SCL_OUT;
	Card42_SDA_OUT;  

	Card42_POWER_H;
	delay15us();
	delay15us();
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SDA_L; 
	Card42_WriteByte(0x30);
	Card42_WriteByte(addr);
	Card42_WriteByte(0x00);
	Card42_SCL_L;
	Card42_SCL_H;
	Card42_SDA_L;
	Card42_SDA_H;
	Card42_SDA_IN;
	for( i=0;i<8;i++)
	{
		Card42_SCL_L;
		delay_ms(1);
		*Stringdata=*Stringdata>>1;
		if(Card42_SDARI)
		{
			*Stringdata=*Stringdata|0x80;
		}
		Card42_SCL_H;		
		delay_ms(1);		
	}
	Card42_POWER_L;
	return(0);
}
/*******************************************************************************
* 函 数 名         : Card42_Verify
* 函数功能		     : 4442卡密码验证
* 输    入         : 密码拆分3个字节分别为data1、data2、data3
* 输    出         : 返回值 0：成功   1： 失败
*******************************************************************************/
u8 Card42_Verify(u8 data1,u8 data2,u8 data3)//PWOER MUST ALWAYS ON,OTHERWISE MUST VERIFY AGAIN
{
	u8 i,errcount=0;
	
	Card42_SCL_OUT;
	Card42_SDA_OUT;
	Card42_SCL_L;
	Card42_SDA_H;			
	Card42_SCL_H;			
	Card42_SDA_L;
	Card42_WriteByte(0x31);
	Card42_WriteByte(0x00);
	Card42_WriteByte(0x00);
	Card42_SCL_L;
	Card42_SCL_H;
	Card42_SDA_L;  
	Card42_SDA_H;			
	Card42_SDA_IN;
	for(i=0;i<8;i++)
	{
		Card42_SCL_L;
		Card42_SCL_H;			
		errcount=errcount>>1;
		if(Card42_SDARI)
		{
			errcount=errcount|0x80;
		}
	}
	
	for(i=0;i<24;i++) //NEXT REFERENCE DATA BYTE 1--3,NO USED BUT MUST REMAIN CLK ..ED
	{
		Card42_SCL_L;
		delay15us();
		Card42_SCL_H;
		delay15us();
	}
	Card42_SDA_OUT;
	//FIND A NON ZERO BIT,MAKE A UPDATE EC DATA
	if((errcount&0x07)==0)
	{
		return 1;		//
	}
	if((errcount&0x01)!=0)
	{
		errcount=errcount&0xfe;
	}
	else
	{
		if((errcount&0x02)!=0)
		{
			errcount=errcount&0xfd;
		}
		else
		{
			errcount=errcount&0xfb;
		}
	}
	Card42_SCL_L;
	Card42_SDA_H;
	Card42_SCL_H;
	Card42_SDA_L;
	Card42_WriteByte(0x39);//
	Card42_WriteByte(0x00);
	Card42_WriteByte(errcount);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SDA_IN;
	for(i=0;i<124;i++)
	{
		Card42_SCL_L;
		delay15us();
		Card42_SCL_H;
		delay15us();
	}
	Card42_SDA_OUT;
	//COMPARE VERIFICATION DATA
	Card42_SCL_L;
	Card42_SDA_H;
	Card42_SCL_H;
	Card42_SDA_L;
	delay15us();
	Card42_WriteByte(0x33);//
	Card42_WriteByte(0x01);
	Card42_WriteByte(data1);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SCL_L;
	Card42_SCL_H;
	Card42_SCL_L;
	Card42_SCL_H;
	//data2
	Card42_SCL_L;
	Card42_SDA_H;
	Card42_SCL_H;
	Card42_SDA_L;
	Card42_WriteByte(0x33);//
	Card42_WriteByte(0x02);
	Card42_WriteByte(data2);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SCL_L;
	Card42_SCL_H;
	Card42_SCL_L;
	Card42_SCL_H;
	//DATA3
	Card42_SCL_L;
	Card42_SDA_H;
	delay15us();
	Card42_SCL_H;
	Card42_SDA_L;
	delay15us();
	Card42_WriteByte(0x33);//
	Card42_WriteByte(0x03);
	Card42_WriteByte(data3);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SCL_L;
	Card42_SCL_H;
	Card42_SCL_L;
	Card42_SCL_H;
	//UPDATE SM
	Card42_SCL_L;
	Card42_SDA_H;
	delay15us();
	Card42_SCL_H;
	Card42_SDA_L;
	delay15us();
	Card42_WriteByte(0x39);//
	Card42_WriteByte(0x00);
	Card42_WriteByte(0xff);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SDA_IN;
	for(i=0;i<124;i++)
	{
		Card42_SCL_L;
		delay15us();
		Card42_SCL_H;
		delay15us();
	}
	Card42_SDA_OUT;
	//COMPARE VERIFICATION DATA
	Card42_SCL_L;
	Card42_SDA_H;
	Card42_SCL_H;
	Card42_SDA_L;
	delay15us();
	Card42_WriteByte(0x31);//
	Card42_WriteByte(0x00);
	Card42_WriteByte(0x00);
	Card42_SCL_L;
	Card42_SCL_H;			
	Card42_SDA_L;
	Card42_SDA_H;
	Card42_SDA_IN;
	for(i=0;i<8;i++)
	{
		Card42_SCL_L;
		Card42_SCL_H;			
		errcount=errcount>>1;
		if(Card42_SDARI)
		{
			errcount=errcount|0x80;
		}
	}
	
	for(i=0;i<24;i++) //NEXT REFERENCE DATA BYTE 1--3,NO USED BUT MUST REMAIN CLK ..ED
	{
		Card42_SCL_L;
		delay15us();
		Card42_SCL_H;
		delay15us();
	}
	Card42_SCL_L;
	Card42_SDA_OUT;
	if((errcount&0x07)==0x07)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


/*******************************************************************************
* 函 数 名         : Card42_WriteString
* 函数功能		     : 写数据
* 输    入         : add：地址 data:数据
* 输    出         : 返回值：0-成功， 1-失败
*******************************************************************************/
u8 Card42_WriteString(u8 add,u8 data)
{
	u8 i=0;
	Card42_SCL_OUT;
	Card42_SDA_OUT;
	Card42_SCL_L;
	Card42_SDA_H;
	Card42_SCL_H;
	Card42_SDA_L;
	Card42_WriteByte(0X38);
	Card42_WriteByte(add);
	Card42_WriteByte(data);
	Card42_SCL_L;
	Card42_SDA_L;
	Card42_SCL_H;
	Card42_SDA_H;
	Card42_SCL_L;;
	Card42_SDARI;
	for(i=0;i<254;i++)
	{
		Card42_SCL_H;
		delay15us();
		Card42_SCL_L;
		delay15us();
	}
	
	Card42_SDA_OUT;
	return 0;
}
/*******************************************************************************
* 函 数 名         : Card42_PreReadCID
* 函数功能		     : 读4442预付费卡id+区域号  
* 输    入         : 结构体：42卡各种信息  卡号加区域号共6个字节
* 输    出         : 无
*******************************************************************************/
void Card42_PreReadCID(Card42_PreInfo *Card4442)
{

  int i=0;
	// u8 code=0;
	 
	u8 cardId[4];//卡号
	u8 areacode[2];//区域号
	Card42_ReadString(0xB,&cardId[0]);
  Card42_ReadString(0xC,&cardId[1]);	 
	Card42_ReadString(0xD,&cardId[2]);
	Card42_ReadString(0xE,&cardId[3]);
	Card42_ReadString(0x8,&areacode[0]);
	Card42_ReadString(0x9,&areacode[1]);
	  
	for ( i=0;i<2;i++)
	{
		Card4442->areacode[i]=areacode[i];
	}
	
	for ( i=0;i<4;i++)
	{
		Card4442->cardId[i]=cardId[i];
	}
	
	Card42_ReadString(0x3C,&Card4442->watersalesamount[0]); //读卡内售水量
	Card42_ReadString(0x3D,&Card4442->watersalesamount[1]);
	Card42_ReadString(0x3E,&Card4442->watersalesamount[2]);
	
	Card42_ReadString(0xA,&Card4442->cflag);
}
 /*******************************************************************************
* 函 数 名         : Card42_Writesalesamount
* 函数功能		     : 写44442预付费卡i售水量
* 输    入         : 上位机返回的三个字节的售水量
* 输    出         :1 写卡成功  0 写卡失败
*******************************************************************************/
u8 Card42_Writesalesamount(u8 *salesamount)
{
	u8 data=0;
	u8 i=0;
	u8 address=0x3C;//地址关键，姚金沅2010929
	for(i=0;i<3;i++)
	{
		Card42_PowerOn();	//开电源
		if(Card42_Verify(0xA3,0x61,0x99)==0)//4442卡密码校验
		{
			delay_us(10);	
			Card42_WriteString(address,salesamount[i]);//写一个字节的售水位			
		} 
		Card42_PowerOff();//关电源
		Card42_ReadString(address,&data);
		if(salesamount[i]!=	data) 
		{
			return 0;//写入和读出数据比较判断写入是否成功
		}
		address++;
	}
  	
	return 1;//写卡成功
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
