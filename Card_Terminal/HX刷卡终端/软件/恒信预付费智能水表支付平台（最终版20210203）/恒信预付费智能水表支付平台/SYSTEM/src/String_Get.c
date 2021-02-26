/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��String_Get.c
 * ��Ŀ���� ���ַ���ȡ���     
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2020-10-29
**********************************************************************************/
#include "String_Get.h"
#include "string.h"

//��ȡĳ���ַ�������
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
//��ȡĳ���ַ����ֵĴ���
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


//��ȡĳһ���ַ�
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
//�������
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
/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************/
