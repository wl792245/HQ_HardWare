/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card57_Config.h
 * 项目描述 ：57卡接口调用       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 硬件连接 ：Card57_CFE--PC0  Card57_OUT--PC1	Card57_POWER--PC3 Card57_ICSW--PC2
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#ifndef __CARD57_CONFIG_H_
#define __CARD57_CONFIG_H_
#include "stm32f10x.h"
#include "IIC_Card.h"

#define	CARD57_MAX      150


#define Card57_CFE       GPIO_Pin_0 
#define Card57_OUT       GPIO_Pin_1 
#define Card57_POWER     GPIO_Pin_3 
#define Card57_ICSW      GPIO_Pin_2 
#define Card57_GPIO      GPIOC 

#define  Card57_OUTPUT_H GPIO_SetBits(Card57_GPIO,Card57_OUT)
#define  Card57_OUTPUT_L GPIO_ResetBits(Card57_GPIO,Card57_OUT)
#define  Card57_CFE_H    GPIO_SetBits(Card57_GPIO,Card57_CFE)
#define  Card57_CFE_L    GPIO_ResetBits(Card57_GPIO,Card57_CFE)
#define  Card57_POWER_H  GPIO_SetBits(Card57_GPIO,Card57_POWER)
#define  Card57_POWER_L  GPIO_ResetBits(Card57_GPIO,Card57_POWER)

#define Card57_Line        EXTI_Line2
#define Card57_IRQn        EXTI2_IRQn    //卡中断
#define Card57_IRQHandler  EXTI2_IRQHandler //中断向量名

//57卡预付费信息结构体
typedef struct C57prepaymentInfo	
{
		u8 cardId[5];//卡号
		u8 cflag;//卡标志位
		u8 watersalesamount[2];//当前售水额 元
		u8 total[2]; //累计销售额	
}Card57_PreInfo;

//57卡阶梯信息结构体
typedef struct C57LadderpaymentInfo	
{
		u8 cardId[5];//卡号
		u8 cflag;//卡标志位
		u8 watersalesamount[4];//售水额（分）		
		u8 ladderprice[6];//价		
		u8 ladderamount[3];//量
		u8 total[3]; 	
}Card57_LadderInfo;





extern volatile uint32_t   Card57_Flag;

void Card57_Init(u16 m_irq, u16 s_irq);

void Card57_PreReadCID(Card57_PreInfo *Card57Pre);
u8 Card57_PreWrite(Card57_PreInfo Card57Pre);

void Card57_LadderReadCID(Card57_LadderInfo *Card57Ladder);
u8 Card57_LadderWriteCID(Card57_LadderInfo Card57Ladder);
#endif
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
