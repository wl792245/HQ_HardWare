/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：CardInfoDeal.h
 * 项目描述 ：数据读取与处理：华旭卡4428和4442
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-2-2
**********************************************************************************/
#ifndef __CARDINFODEAL_H_
#define __CARDINFODEAL_H_

#include "stm32f10x.h"
#include "HXCard_Config.h"
#include "SCCard_Config.h"

typedef struct CARDPC
{
	char Card_PCRecHead[2];                 //PC返回DLL头
  char Card_PCRecStr[200];                //PC返回DLL内容
	char Card_iCardType[4];               //三川卡类型
	char Card_iManageCode[9];             //三川河间维护码
	char Card_iKey[15];                   //三川河间密钥
	char Card_CardId[9];                  //三川卡号
	char Card_BiaoId[9];                  //三川表号
	char Card_BuyNum[3];                  //三川充值序号
	char Card_CardMoneny[8];              //三川卡上金额（分）
	char Card_BuyAllNum[4];               //三川充值总序号
	char Card_UseWater[7];                //三川用水量0.1吨
	char Card_ZhuangTaiMa[9];             //三川状态码
}Card_PC, *PCard_PC;

typedef struct CARDSERVER
{
	char Card_ServerRecHead[18];        //服务器取头
  PSCCard_Server SCCard_Info;         //三川卡信息
}Card_Server, *PCard_Server;

void SCCard_Deal(void);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
