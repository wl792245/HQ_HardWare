/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：utils.h
 * 项目描述 ：字符处理调用
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-11
**********************************************************************************/
#ifndef __UTILS_H_
#define __UTILS_H_

void Byte_Split(unsigned char *pDest, unsigned char *pSrc, int len);
void HexToChar(unsigned char *pDest, unsigned char *pSrc, int len);
void StrToHex( unsigned char *pbDest, unsigned char *pbSrc, int nLen);
void  StrToDec(unsigned char *pbDest, char *pbSrc);
int StringToInt(char* pbSrc);
#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
