/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：SCCard_Config.h
 * 项目描述 ：三川卡模块功能定义     
 * 开发平台 ：基于STM32F103CBT6芯片开发
 * 硬件连接 ：SC(CS)--PB5
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-2-3
**********************************************************************************/
#ifndef __SCCARD_CONFIG_H_
#define __SCCARD_CONFIG_H_

#include "stm32f10x.h"


#define SCCARD_INIT_Port          GPIOB
#define SCCARD_INIT_CSW           GPIO_Pin_5
#define SCCARD_Line               EXTI_Line5
#define SCCARD_INIT_IRQn          EXTI9_5_IRQn
#define SCCARD_INIT_IRQHandler    EXTI9_5_IRQHandler

extern volatile uint32_t   SCCardIsInter;

typedef struct SCCARD5557PC
{
	char PCRecHead[2];                 //PC返回DLL头
  char PCRecStr[200];                //PC返回DLL内容
	char iCardTypeSC[4];               //三川卡类型
	char iManageCodeSC[9];             //三川河间维护码
	char iKeySC[15];                   //三川河间密钥
	char CardIdSC[9];                  //三川卡号
	char BiaoIdSC[9];                  //三川表号
	char BuyNumSC[3];                  //三川充值序号
	char CardMonenySC[8];              //三川卡上金额（分）
	char BuyAllNumSC[4];               //三川充值总序号
	char UseWaterSC[7];                //三川用水量0.1吨
	char ZhuangTaiMaSC[9];             //三川状态码
}SCCard_5557PC, *PSCCard_5557PC;

typedef struct SCCARD5557SERVER
{
	char ServerRecHead[18];             //服务器取头
	char SCID[11];                     	//机器号
	char SC5557CardID[17];             	//三川5557卡号
	char SC5557OrderId[60];            	//三川订单号
	char iPrice1SC[10];									//三川价格1 0.1分
	char iPrice2SC[10];									//三川价格2 0.1分
	char iPrice3SC[10];									//三川价格3 0.1分
	char iTon1SC[5];                    //三川吨限1 若是只有二阶价则0-9999，三阶价则0-99
	char iTon2SC[5];                    //三川吨限2 0-99
	char BuyMonenySC[10];               //三川充值金额
	char SC5557UserID[17];              //三川5557用户编号
	char SCAdress[60];                  //三川用户i地址
	char SCXingzhi[30];                 //三川用水性质
	char SCTotalamount[10];             //三川水表总量
	char SCLastAllBuyMount[10];         //三川当年用水总量
}SCCard_Server, *PSCCard_Server;

void SCCard_Init(u16 m_irq, u16 s_irq);
u8 SCCard_PCDataSpit(PSCCard_5557PC SCCardPC,char *pdst_ServerSend,char *psrc_PCRecv);
void SCCard_ServerDataSpit(PSCCard_Server SCCardServer,const SCCard_5557PC SCCardPC, char *pdst_ServerSend, char *pdst_PCRec, char *psrc_Server, char * PCSend);
void SCCard_ServerDataCombine(char *pdst_ServerSend, const SCCard_Server SCCardServer, char * WriteCardSuccess, int isSuccess);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
