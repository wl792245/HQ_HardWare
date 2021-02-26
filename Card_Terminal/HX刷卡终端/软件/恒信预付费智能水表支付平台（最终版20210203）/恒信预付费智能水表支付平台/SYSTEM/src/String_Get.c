/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************
 * 文件名   ：String_Get.c
 * 项目描述 ：字符获取设计     
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2020-10-29
**********************************************************************************/
#include "String_Get.h"
#include "string.h"

//获取某个字符的数量
void Char_Find(unsigned int* Destination, const char* Source, char Single_Char)
{
    unsigned int i, j;
   
    for (j = 0,i = 0; i < strlen(Source); i++)
    {
        if (Source[i] == Single_Char)
        {
            Destination[j++] = i;
        }
    }
    Destination[j] = '\0';
}
//获取某个字符出现的次数
unsigned int Char_Num(const char* Source, char Single_Char)
{
    unsigned int i;
    unsigned int time = 0;
   
    for (i = 0; i < strlen(Source); i++)
    {
        if (Source[i] == Single_Char)
        {
            time +=1;
        }
    }
    return time;
}


//获取某一段字符
void String_Find(char* Destination, const char* Source, int initial, int end)
{
    unsigned int i, len;
    len = end - initial;
    for (i = 0; i < len; i++)
    {
        Destination[i] = Source[i+initial];
    }
    Destination[i] = '\0';
}
//数组清除
void String_Clear(char* Destination, int len)
{
    unsigned int i;
    for (i = 0; i < len; i++)
    {
        Destination[i] = '\0';
    }

}
void Interger_Clear(unsigned int* Destination, int len)
{
	  unsigned int i;
    for (i = 0; i < len; i++)
    {
        Destination[i] = 0;
    }
}
/******************** (C) COPYRIGHT 2020 江苏恒沁科技有限公司 ********************/
