/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：CardInfoDeal.h
 * 项目描述 ：数据读取与处理：42卡.57卡.24卡
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#ifndef __CARDINFODEAL_H_
#define __CARDINFODEAL_H_

#include "stm32f10x.h"
#include "IIC_Card.h"
#include "Card42_Config.h"
#include "Card24_Config.h"
#include "Card57_Config.h"
//24卡.42卡电源引脚连接
#define Card_POWER       GPIO_Pin_5 //24.42卡电源脚
//24卡.42卡中断引脚连接
#define Card_ICSW        GPIO_Pin_9 //24.42卡中断脚
#define Card_GPIO        GPIO_I2C 

#define Card_Line        EXTI_Line9
#define Card_IRQn        EXTI9_5_IRQn    //卡中断
#define Card_IRQHandler  EXTI9_5_IRQHandler //中断向量名

//卡十六进制半字节数组
typedef struct CardHex
{
	u8 hexcid[15];          //卡号十六进制半字节数组
	u8 hexaid[15];          //区域码十六进制半字节数组
	u8 hexamount[15];       //卡内余量十六进制半字结数组
  u8 hexbuyamount[15];    //充值量十六进制半字节数组
	u8 hextotal[15];        //总量十六进制
	u8 hextime[15];         //次数十六进制
	u8 hexsecurity[15];     //保密位十六进制
  u8 hexoneprice[15];     //一价十六进制
	u8 hextwoprice[15];     //二价十六进制
	u8 hexthreeprice[15];   //三价十六进制
	u8 hexfourprice[15];    //四价十六进制
	u8 hexoneamount[15];    //一量十六进制
	u8 hextwoamount[15];    //二量十六进制
	u8 hexthreeamount[15];  //三量十六进制
}Card_Hex;

//卡十六进制转字符数组
typedef struct CardChar
{
	u8 strcid[15];          //卡号十六进制转字符数组
	u8 straid[15];          //区域码十六进制转字符数组
	u8 stramount[15];       //卡内余量十六进制转字符数组
  u8 strbuyamount[15];    //充值量十六进制转字符数组
	u8 strtotal[15];        //总量字符
	u8 strtimes[15];        //充值次数字符
	u8 strsecurity[15];     //保密位字符
	u8 stroneprice[15];     //一价字符
	u8 strtwoprice[15];     //二价字符
	u8 strthreeprice[15];   //三价字符
	u8 strfourprice[15];    //四价字符
	u8 stroneamount[15];    //一量字符
	u8 strtwoamount[15];    //二量字符
	u8 strthreeamount[15];  //三量字符
}Card_Char;

//卡处理发送信息结构体
typedef struct CardDeal
{
	char TSendCID[100];          //上传到服务器的字符串
	u8 *Card_Head;              //卡头字符
	u8 Card_Flag;               //卡刷卡成功标志位 0失败 1成功
	Card_Hex Card_HexInfo;      //卡十六进制半字节数组
  Card_Char Card_CharInfo;    //卡十六进制转字符数组
}Card_DealSend;

//卡处理接收信息结构体
typedef struct CardDealRecv
{
	char head[15];             //服务器返回字符串的头 用来判断是什么类型的卡
	char terminalId[20];        //机器号
	char cardId[15];           //卡号
	char orderId[40];          //订单号
	char userId[40];           //用户号
	char useradress[40];       //用户地址
	char userproperties[40];   //用水性质
	char Totalamount[40];      //用户水表中水的总量
	char LastAllBuyMount[40];  //用户当年购水量
	char Buyamout[40];         //充值量
	char total[15];             //总量
	char pricemountstr[30];    //充值量字符串
	char times[15];             //次数
	char security[15];          //保密位
	char oneprice[15];          //一价
	char twoprice[15];          //二价
	char threeprice[15];        //三价
	char fourprice[15];         //四价
	char oneamount[15];         //一量
	char twoamount[15];         //二量
	char threeamount[15];       //三量
}Card_DealRecv;

extern volatile uint32_t   CardIsInter;

void Card_Init(u16 m_irq, u16 s_irq);
u8 CopyCIDTo24C02(void);
u8 CopyScrrenvTo24C02(void);
u8 CopyTerminalVTo24C02(void);
u8 CopyIPPortTo24C02(void);
u8 CopyQRcodeTo24C02(void);

void Card24_PDeal(Card24_PreInfo *Info24p);
void Card24_LDeal(Card24_LadderInfo *Info24l);
void Card42_PDeal(Card42_PreInfo *Info42p);
void Card57_PDeal(Card57_PreInfo * Info57p);
void Card57_LDeal(Card57_LadderInfo *Card57l);
void CacheOrder_update(void);

#endif

/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
