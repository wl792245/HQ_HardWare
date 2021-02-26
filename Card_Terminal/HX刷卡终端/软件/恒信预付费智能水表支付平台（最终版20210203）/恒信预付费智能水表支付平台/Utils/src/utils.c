/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��utils.c
 * ��Ŀ���� ���ַ�������
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-11
**********************************************************************************/
#include "utils.h"
#include "stm32f10x.h"
#include "math.h" 
#include "ctype.h"
#include "stdlib.h"
/*******************************************************************************
* �� �� ��         : Byte_Split
* ��������		     : �ֽڲ��
* ��    ��         : pDest��Ŀ�����飬 pSrc��Դ���飻len����ֳ���
* ��    ��         : ��
*******************************************************************************/
void Byte_Split(unsigned char *pDest, unsigned char *pSrc, int len)
{
	unsigned char i;
	for(i = 0; i < len/2; i++)
	{
		pDest[2*i] = pSrc[i]>>4; //��4λ
		pDest[2*i+1] = pSrc[i]&0xf;    //��4λ
	}
	pDest[len] = '\0';
}
/*******************************************************************************
* �� �� ��         : HexToChar
* ��������		     : 16����ת�ַ�
* ��    ��         : pDest��Ŀ�����飬 pSrc��Դ���飻len�����ݳ���
* ��    ��         : ��
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
* �� �� ��         : StrToHex
* ��������		     : �ַ�ת16����
* ��    ��         : pDest��Ŀ�����飬 pSrc��Դ���飻len�����ݳ���
* ��    ��         : ��
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
* �� �� ��         : StrToDec
* ��������		     : �ַ�ת10����
* ��    ��         : pbSrc:Դ����
* ��    ��         : ��
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
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
