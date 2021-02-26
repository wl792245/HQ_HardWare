/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：utils.c
 * 项目描述 ：字符处理定义
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-11
**********************************************************************************/
#include "utils.h"
#include "stm32f10x.h"
#include "math.h" 
#include "ctype.h"
#include "stdlib.h"
/*******************************************************************************
* 函 数 名         : Byte_Split
* 函数功能		     : 字节拆解
* 输    入         : pDest：目标数组， pSrc：源数组；len：拆分长度
* 输    出         : 无
*******************************************************************************/
void Byte_Split(unsigned char *pDest, unsigned char *pSrc, int len)
{
	unsigned char i;
	for(i = 0; i < len/2; i++)
	{
		pDest[2*i] = pSrc[i]>>4; //高4位
		pDest[2*i+1] = pSrc[i]&0xf;    //低4位
	}
	pDest[len] = '\0';
}
/*******************************************************************************
* 函 数 名         : HexToChar
* 函数功能		     : 16进制转字符
* 输    入         : pDest：目标数组， pSrc：源数组；len：数据长度
* 输    出         : 无
*******************************************************************************/
void HexToChar(unsigned char *pDest, unsigned char *pSrc, int len)
{
	unsigned char i;
	for (i=0; i<len;i++)
	{
		if(pSrc[i] <= 0x09)
		{
			pDest[i] = pSrc[i]+'0'; 
		}
		else if ((pSrc[i] <= 0x0F) && (pSrc[i] >= 0x0A))
		{
			pDest[i] = pSrc[i] - 0X0A +'A'; 
		}
		else
		{
			pDest[i] = '0';
		}
	}
	pDest[i] = '\0';
}
/*******************************************************************************
* 函 数 名         : StrToHex
* 函数功能		     : 字符转16进制
* 输    入         : pDest：目标数组， pSrc：源数组；len：数据长度
* 输    出         : 无
*******************************************************************************/
void StrToHex( unsigned char *pbDest, unsigned char *pbSrc, int nLen)
{
	char h1,h2;
	uint8_t s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}
/*******************************************************************************
* 函 数 名         : StrToDec
* 函数功能		     : 字符转10进制
* 输    入         : pbSrc:源数组
* 输    出         : 无
*******************************************************************************/
void  StrToDec(unsigned char *pbDest, char *pbSrc)
{
	char h1,h2;
	uint8_t s1,s2;

	h1 = pbSrc[0];
	h2 = pbSrc[1];
	if ((h1==0x20)&&(h2==0x20))
		pbDest[0]=0;
	if ((h1==0x20)&& (h2!=0x20))
	{
		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;
		pbDest[0] = s2;
	}
	if ((h2==0x20)&& (h1!=0x20))
	{
		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;
		pbDest[0] = s1;
	}
	if ((h2!=0x20)&& (h1!=0x20))
	{
		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;
		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;
		pbDest[0] = s1*10 + s2;
	}
}
int StringToInt(char* pbSrc)
{
  const char* ptr = pbSrc;
  int priceNum = 0;
  while (*ptr != '\0')
  {
    priceNum *= 16;
    if((*ptr >= '0') && (*ptr <= '9'))
      priceNum += *ptr - '0';
    if ((*ptr >= 'A') && (*ptr <= 'Z'))
      priceNum += *ptr - 'A';
    if ((*ptr >= 'a') && (*ptr <= 'z'))
      priceNum += *ptr - 'a'+ 10;
    ptr++;
  }
	return priceNum;
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
