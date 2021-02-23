/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：String_Get.c
 * 项目描述 ：字符获取接口声明     
 * 开发平台 ：基于STM32F103C8T6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2020-10-29
**********************************************************************************/
#ifndef _STRING_CONFIG_H
#define _STRING_CONFIG_H

void Char_Find(unsigned int* Destination, const char* Source, char Single_Char);
void String_Find(char* Destination, const char* Source, int initial, int end);
unsigned int Char_Num(const char* Source, char Single_Char);
unsigned int Arrary_Zero(const char* Source, char Num);
void String_Clear(char* Destination, int len);
void Interger_Clear(unsigned int* Destination, int len);
void Get_Head(char *pdest, const char *psrc,char SplitChar, unsigned int *num, int len_num);
#endif
/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************/
