/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：AT24C02.c
 * 项目描述 ：开发板上24C02接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-6
**********************************************************************************/
#include "AT24C02.h"
#include "IIC_24C02.h"
#include "systick.h"
#include "String_Get.h"
#include "string.h"

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
volatile int Restart_Count = 0;
char TerminalIDServerIP[100];	                                                           //存放在系统24c02的机器号和服务器ip
/*******************************************************************************
* 函 数 名         : AT24C02_Init
* 函数功能		     : AT24C02初始化   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void AT24C02_Init(void)
{
	IIC_INIT();
}
/*******************************************************************************
* 函 数 名         : AT24C02_ReadOneByte
* 函数功能		     : 24c02读一个字节地址  数据	   
* 输    入         : addr
* 输    出         : 返回值temp
*******************************************************************************/
static u8 AT24C02_ReadOneByte(u16 addr)
{
	u8 temp=0;

	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
		IIC_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0xA1);
	IIC_Wait_Ack();
	temp=IIC_Read_Byte(0); //  0   代表 NACK
	IIC_NAck();
	IIC_Stop();
	return temp;	
}

/*******************************************************************************
* 函 数 名         : AT24C02_WriteOneByte
* 函数功能		     : 24c02写一个字节地址  数据	   
* 输    入         : addr  dt
* 输    出         : 0，写成功，1:写失败
*******************************************************************************/
static u8 AT24C02_WriteOneByte(u16 addr,u8 dt)
{
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0xA0);
		IIC_Wait_Ack();
		IIC_Send_Byte(addr>>8);	//发送数据地址高位
	}
	else
	{
	   IIC_Send_Byte(0xA0+((addr/256)<<1));//器件地址+数据地址
	}
	while(IIC_Wait_Ack())
	{
		return 1;
	}
		
	IIC_Send_Byte(addr%256);//双字节是数据地址低位		
							//单字节是数据地址低位
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
* 函 数 名         : AT24C02_Read
* 函数功能		     : 在AT24C02里面的指定地址开始读出指定个数的数据
* 输    入         : ReadAddr :开始读出的地址 对24c02为0~255	  
                     pBuffer  :数据数组首地址
                     NumToRead:要读出数据的个数
* 输    出         : 无
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
* 函 数 名         : AT24C02_Write
* 函数功能		     : 在AT24C02里面的指定地址开始写入指定个数的数据
* 输    入         : WriteAddr:开始写入的地址 对24c02为0~255
                     pBuffer  :数据数组首地址
                     NumToRead:要读出数据的个数
* 输    出         : 0:写成功，1：写失败
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
* 函 数 名         : AT24C02IPTid_Init
* 函数功能		     : 从AT24C02里面读取设备信息和服务器信息
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void AT24C02IPTid_Init(void)
{
	u32 num[10] = {0};                                                                       //查找分隔符的个数
	char isFirst[3]={'0'};
	String_Clear(TerminalIDServerIP, 100);
	AT24C02_Read(98, isFirst, 2);
	if (strstr(isFirst, FISRT) == NULL)
	{ 
		strcpy(TerminalIDServerIP,FTerminalIDServerIP);
		AT24C02_Write(5,TerminalIDServerIP,strlen(TerminalIDServerIP));
		strcpy(isFirst, FISRT);
		AT24C02_Write(98,isFirst,2);
		ResStart_Write(DeviceInfo.TerminalInfo.ReStart, 0);  //重启次数0
	}
	else
	{
			AT24C02_Read(5,TerminalIDServerIP,99);
	}
	Char_Find(num,TerminalIDServerIP, '%');                                                  //获取百分号的位置
	String_Find(DeviceInfo.TerminalInfo.TerminalId, TerminalIDServerIP, 0, num[0]);          //获取设备机器号
	String_Find(DeviceInfo.TerminalInfo.ICCID, TerminalIDServerIP, num[0]+1, num[1]);        //获取设备ICCID
	String_Find(DeviceInfo.TerminalInfo.ScreenV, TerminalIDServerIP, num[1]+1, num[2]);      //获取液晶屏版本号
	String_Find(DeviceInfo.TerminalInfo.TerminalV, TerminalIDServerIP, num[2]+1, num[3]);    //获取设备版本号
	String_Find(DeviceInfo.ServerInfo.ServerIP, TerminalIDServerIP, num[3]+1, num[4]);       //获取服务器IP号
	String_Find(DeviceInfo.ServerInfo.ServerPort, TerminalIDServerIP, num[4]+1, num[5]);     //获取服务器端口号
	String_Find(DeviceInfo.TerminalInfo.QRcode, TerminalIDServerIP, num[5]+1, num[6]);       //获取设备二维码

}
/*******************************************************************************
* 函 数 名         : ResStartM6311
* 函数功能		     : 记录M6311重启次数
* 输    入         : ReStartInfo：重启次数转字符存储，value：重启值
* 输    出         : 无
*******************************************************************************/
void ResStart_Write(char *ReStartInfo, int value)//记录重连6311
{
	ReStartInfo[0]=value/1000 + '0';
	ReStartInfo[1]=value%1000/100+'0';
	ReStartInfo[2]=value%100/10+'0';
	ReStartInfo[3]=value%10+'0';
	ReStartInfo[4]='\0';
	AT24C02_Write(100,ReStartInfo,5);
}
/*******************************************************************************
* 函 数 名         : ReStart_Read
* 函数功能		     : 
* 输    入         : ReStartInfo：重启次数转字符存储
* 输    出         : int :返回读取到的次数整形
*******************************************************************************/
int ReStart_Read(char *ReStartInfo)
{
	int value = 0;
	const char* ptr = ReStartInfo;
	AT24C02_Read(100,ReStartInfo,5);
	while(*ptr != '\0')
	{
		value *= 10;
    value += *ptr - '0';
		ptr++;
	}
	return value;
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
