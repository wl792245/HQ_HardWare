/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��utils.h
 * ��Ŀ���� ���ַ��������
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-11
**********************************************************************************/
#ifndef __UTILS_H_
#define __UTILS_H_

void Byte_Split(unsigned char *pDest, unsigned char *pSrc, int len);
void HexToChar(unsigned char *pDest, unsigned char *pSrc, int len);
void StrToHex( unsigned char *pbDest, unsigned char *pbSrc, int nLen);
void  StrToDec(unsigned char *pbDest, char *pbSrc);
int StringToInt(char* pbSrc);
#endif

/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
