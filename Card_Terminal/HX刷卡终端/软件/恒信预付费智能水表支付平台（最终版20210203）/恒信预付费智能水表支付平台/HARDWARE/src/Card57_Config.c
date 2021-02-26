/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************
 * 文件名   ：Card57_Config.c
 * 项目描述 ：57卡通讯接口设计       
 * 开发平台 ：基于STM32F103ZET6芯片开发
 * 作  者   ：Terry  QQ: 792245610
 * 时  间   ：2021-1-7
**********************************************************************************/
#include "Card57_Config.h"
#include "stm32f10x_exti.h"
#include "systick.h"
#include "string.h"

volatile uint32_t   Card57_Flag=0; //57卡中断标志位
/*******************************************************************************
* 函 数 名         : Card57_Init
* 函数功能		     : 57卡引脚初始化  
* 输    入         : m_irq：抢占优先级（1），s_irq：响应优先级（0）
* 输    出         : 无
*******************************************************************************/
void Card57_Init(u16 m_irq, u16 s_irq)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); /* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=Card57_OUT;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* 初始化GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_CFE;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	  //设置浮空输入模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* 初始化GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_POWER;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* 初始化GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_ICSW ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* 初始化GPIO */	
		
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);//选择GPIO管脚用作外部中断线路
	//此处一定要记住给端口管脚加上中断外部线路
	/* 设置外部中断的模式 */ 
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* 设置NVIC参数 */	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
	NVIC_InitStructure.NVIC_IRQChannel = Card57_IRQn; 	//打开EXTI2的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //使能
	NVIC_Init(&NVIC_InitStructure); 
}
/*******************************************************************************
* 函 数 名         : Card57_PowerOn
* 函数功能		     : 打开57卡电源  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card57_PowerOn(void)
{
	u8 i=0;
	Card57_POWER_H;
	for(i=0; i<10; i++) 
	{
		delay_us(15);
	}
}
/*******************************************************************************
* 函 数 名         : Card57_PowerOff
* 函数功能		     : 关闭57卡电源  
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Card57_PowerOff(void)
{
	Card57_POWER_L;
}
/*******************************************************************************
* 函 数 名         : delay192
* 函数功能		     : 延时192微秒 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void delay192(void)
{
	delay_us(192);
}
/*******************************************************************************
* 函 数 名         : delay400
* 函数功能		     : 延时400微秒 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void delay400(void)
{
	delay_us(400);
}
/*******************************************************************************
* 函 数 名         : delay448
* 函数功能		     : 延时448微秒 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
static void delay448(void)
{
	delay_us(448);
}
/*******************************************************************************
* 函 数 名         : Card57_Writebit
* 函数功能		     : 对57卡的位进行操作
* 输    入         : data:数据
* 输    出         : 无
*******************************************************************************/
void Card57_Writebit(u8 bit)
{
	if (bit==1)
	{
		Card57_OUTPUT_L;
		delay448();
    Card57_OUTPUT_H;
		delay192();
	}
	else
	{
		Card57_OUTPUT_L;
		delay192();
    Card57_OUTPUT_H;
		delay192();
	}
} 
/*******************************************************************************
* 函 数 名         : Card57_WritePassword
* 函数功能		     : 对57卡的进行写密码操作
* 输    入         : data:指向整形数据的指针
* 输    出         : 无
*******************************************************************************/
void  Card57_WritePassword(unsigned int data1, unsigned int data2)
{
	u8 i;
	for (i=0; i<16; i++)
	{
		if ((data1 &0x8000)>0)
		{
			Card57_Writebit(1);
		}
		else
		{
			Card57_Writebit(0);
		}
		data1 = data1 << 1;
	}
	for (i=0; i<16; i++)
	{
		if ((data2 &0x8000)>0)
		{
			Card57_Writebit(1);
		}
		else
		{
			Card57_Writebit(0);
		}
		data2 = data2 << 1;
	}
	return;
}

/*******************************************************************************
* 函 数 名         : Card57_Read
* 函数功能		     : 读取57卡中的数据
* 输    入         : page:页, block1：行，data1：数值
* 输    出         : 无
*******************************************************************************/
u8 Card57_Read(u8 page,u8 block1,u32 *data1)
{
	unsigned char i,j;
	unsigned char x;
	unsigned long data;
	unsigned int outtime;
	u32 count;
  u8 errtag;
	u32 atime[CARD57_MAX];
	u32 btime[CARD57_MAX];
	errtag=0;
	Card57_OUTPUT_H;
	delay_ms(10);
	for(i=0;i<150;i++)
	{
		atime[i]=0x00;
		btime[i]=0x00;
	}
	Card57_OUTPUT_L;
	delay_ms(5);
	Card57_OUTPUT_H;
	delay400();
	Card57_Writebit(0);//reset
	Card57_Writebit(0);
	Card57_OUTPUT_L;
	delay_ms(6);
	Card57_OUTPUT_H;
	delay400();
	Card57_Writebit(1);//read page1
	Card57_Writebit(page);
	Card57_WritePassword(0x8899,0x8644);
	Card57_Writebit(0); //L
	if((block1&0x04)==0)
		Card57_Writebit(0);//block
	else
		Card57_Writebit(1);
		
	if((block1&0x02)==0)
		Card57_Writebit(0);
	else
		Card57_Writebit(1);
		
	if((block1&0x01)==0)
		Card57_Writebit(0);
	else
		Card57_Writebit(1);
		
	Card57_OUTPUT_L;
	i=0;
	j=0;
	data=0;
	x=0;
	j++;
	delay_ms(20);

	j++;
	outtime=0;

	while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_CFE)==1) 
	
	if(outtime++>(1200)) 
	{
		errtag=11;
		goto error;
	}


	while(j!=150)
	{
		SysTick->LOAD=9000;		 
		SysTick->CTRL=0X01;		 
		SysTick->VAL=0;	
	lowlevel:
		outtime=89500;
		while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_CFE)==0) if(outtime--==0) goto error;

		count=SysTick->LOAD-SysTick->VAL;
		if(count<100) goto lowlevel;
		SysTick->CTRL=0;	//关闭计数器
		SysTick->VAL=0;		//清空计数器

		atime[j]=(count/9);

		SysTick->LOAD=9000;		 
		SysTick->CTRL=0X01;		 
		SysTick->VAL=0;	
		
	highlevel:
		outtime=0x3ffff;
		while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_CFE)==1) if(outtime--==0) goto error;
		
		count=SysTick->LOAD-SysTick->VAL;
		if(count<100) goto highlevel;
		
		SysTick->CTRL=0;	//关闭计数器
		SysTick->VAL=0;		//清空计数器

		btime[j++]=(count/9);
	}


	j=0;i=0;
	while(j++!=150) if((btime[j]>(300))&&(btime[j]<(400))) 
	{
		//j++;
		break;
	}
	
	if(j==150)
	{
		errtag=1;
		goto error;
	}
	
	if((atime[j]<(80))||(atime[j]>(170)))//142
	{
		errtag=2;
		goto error;
	}
	j++;
	if((atime[j]<(80))||(atime[j]>(170)))
	{
		errtag=3;
		goto error;
	}      
	if((btime[j]<(100))||(btime[j]>(390)))//76-300
	{
		errtag=4;
		goto error;
	}
	
	if(btime[j]>(300))//205
	{
		data=data<<1;
		data++;
		i++;
		x=0;
	}
	else
	{
		x=1;
	}
	
	while(i<32)
	{
		j++;
		if(j==150)
		{
			errtag=5;
			goto error;
		}
		if((atime[j]<(80))||(atime[j]>(300)))
		{
			errtag=6;
			goto error;
		}
		
		if(atime[j]<(175))//120
		{
			x++;
			if((x&0x01)==0)
			{
				data=data<<1;
				i++;
				if(i==32)  break;
			}
		}
		else
		{
			x=x+2;
			data=data<<1;
			i++;
			if(i==32) break;
		}
		
		if((btime[j]<(100))||(btime[j]>(300)))//gai
		{
			errtag=7;
			goto error;
		}
		if(btime[j]<(205))//120
		{
			x++;
			if((x&0x01)==0)
			{
				data=data<<1;
				data++;
				i++;
				if(i==32) break;
			}
		}
		else
		{
			x=x+2;
			data=data<<1;
			data++;
			i++;
			if(i==32) break;
		}
	}
	*data1=data;

	Card57_OUTPUT_H;
	return 1;
	
error:
	Card57_OUTPUT_H;
	
	if(errtag==1) 
		return 0;
	else 
		return errtag;
}
/*******************************************************************************
* 函 数 名         : Card57_Write
* 函数功能		     : 给57卡写入数据  
* 输    入         :  block1：行，data1-4：数值
* 输    出         : 返回值
*******************************************************************************/
u8 Card57_Write(u8 block1,u8 data1,u8 data2,u8 data3,u8 data4)
{
	u32 rda1=0;
	u32 rdal2=(data1<<24)+(data2<<16)+(data3<<8)+data4;
	Card57_OUTPUT_L;
	delay_ms(5);
	Card57_OUTPUT_H;
	delay400();
	Card57_Writebit(0);//reset
	Card57_Writebit(0);
	Card57_OUTPUT_L;
	delay_ms(5);
	Card57_OUTPUT_H;
	delay400();
	Card57_Writebit(1);//read page0
	Card57_Writebit(0);
	Card57_WritePassword(0x8899,0x8644);//密码处理
	Card57_Writebit(0); //L
	Card57_WritePassword((data1<<8)+data2,(data3<<8)+data4);
	if((block1&0x04)==0)
		Card57_Writebit(0);//block
	else
		Card57_Writebit(1);
	
	if((block1&0x02)==0)
		Card57_Writebit(0);
	else
		Card57_Writebit(1);
		
	if((block1&0x01)==0)
		Card57_Writebit(0);
	else
		Card57_Writebit(1);
		
	Card57_OUTPUT_L;
	delay_ms(6);


		Card57_Read(0X00,block1,&rda1);
		Card57_OUTPUT_H;
		if(rda1!=rdal2)
			return 0;
		
		return 1;


}
/*******************************************************************************
* 函 数 名         :	Card57_PreReadCID
* 函数功能		     :	读5557预付费卡ID
* 输    入         :	无
* 输    出         :	Card57_PreInfo
 *******************************************************************************/
void Card57_PreReadCID(Card57_PreInfo *Card57Pre)
{
	u32 lock1=0;
	u32 lock2=0;
	u32 lock3=0;
	u32 check1lock1=0;
	u32 check1lock2=0;
	u32 check1lock3=0;
	u32 check2lock1=0;
	u32 check2lock2=0;
	u32 check2lock3=0;
	u32 check3lock1=0;
	u32 check3lock2=0;
	u32 check3lock3=0;
	
	Card57_PowerOn();
	Card57_Read(0x0,0x1,&lock1);
	Card57_Read(0x0,0x2,&lock2);
	Card57_Read(0x0,0x3,&lock3);
	Card57_Read(0x0,0x1,&check1lock1);
	Card57_Read(0x0,0x2,&check1lock2);
	Card57_Read(0x0,0x3,&check1lock3);
	Card57_Read(0x0,0x1,&check2lock1);
	Card57_Read(0x0,0x2,&check2lock2);
	Card57_Read(0x0,0x3,&check2lock3);
	Card57_Read(0x0,0x1,&check3lock1);
	Card57_Read(0x0,0x2,&check3lock2);
	Card57_Read(0x0,0x3,&check3lock3);
	
	if (check1lock1==check2lock1)  lock1=check2lock1;
	if (check3lock1==check2lock1)  lock1=check2lock1;
	if (check3lock1==check1lock1)  lock1=check1lock1;
	if (check1lock2==check2lock2)  lock2=check2lock2;
	if (check3lock2==check2lock2)  lock2=check2lock2;
	if (check3lock2==check1lock2)  lock2=check1lock2;
	if (check1lock3==check2lock3)  lock3=check2lock3;
	if (check3lock3==check2lock3)  lock3=check2lock3;
	if (check3lock3==check1lock3)  lock3=check1lock3; 
	
	Card57Pre->cardId[4]=(lock2>>16)&0xFF;
	Card57Pre->cardId[3]=(lock2>>24)&0xFF;
	Card57Pre->cardId[2]=(lock1)&0xFF;
	Card57Pre->cardId[1]=(lock1>>8)&0xFF;
	Card57Pre->cardId[0]=(lock1>>16)&0xFF;
	Card57Pre->cflag=(lock1>>24)&0xFF;	//读卡标志位
	Card57Pre->watersalesamount[0]=(lock3>>24)&0xFF;
	Card57Pre->watersalesamount[1]=(lock3>>16)&0xFF;
	Card57Pre->total[0]=(lock3>>8)&0xFF;
	Card57Pre->total[1]=(lock3)&0xFF;
	Card57_PowerOff();
}
/*******************************************************************************
* 函 数 名         :  Card57_PreWrite
* 函数功能		     :  写5557预付费卡
* 输    入         :  无
* 输    出         :  1 写卡成功  0 写卡失败
 *******************************************************************************/
u8 Card57_PreWrite(Card57_PreInfo Card57Pre)
{
	u8 flag=0;
	u8 flag2=0;
	Card57_PowerOn();
	delay_ms(1000);
	flag= Card57_Write(3,Card57Pre.watersalesamount[0],Card57Pre.watersalesamount[1],
	Card57Pre.total[0],Card57Pre.total[1]);//
	
	delay_ms(1000);
	flag2= Card57_Write(6,0x0,0x0,0x0,0x0);//20-23清零
	if(flag==0||flag2==0)
	{
		return 0 ;//读卡失败
	}
	Card57_PowerOff();
	return 1;//读卡成功
}
/*******************************************************************************
* 函 数 名         : Card57_LadderReadCID
* 函数功能		     : 读57阶梯卡id
* 输    入         : 无
* 输    出         : 卡号共5个字节
*******************************************************************************/
void Card57_LadderReadCID(Card57_LadderInfo *Card57Ladder)
{

	u32 lock1=0;
	u32 lock2=0;
	u32 lock3=0;
	u32 lock4=0;
	u32 check1lock1=0;
	u32 check1lock2=0;
	u32 check1lock3=0;
	u32 check1lock4=0;
	u32 check2lock1=0;
	u32 check2lock2=0;
	u32 check2lock3=0;
	u32 check2lock4=0;
	u32 check3lock1=0;
	u32 check3lock2=0;
	u32 check3lock3=0;	 
	u32 check3lock4=0;

	// u8 cid[5];
	Card57_PowerOn();
	Card57_Read(0x0,0x1,&lock1);
	Card57_Read(0x0,0x2,&lock2);
	Card57_Read(0x0,0x6,&lock3);
	Card57_Read(0x0,0x3,&lock4);
	Card57_Read(0x0,0x1,&check1lock1);
	Card57_Read(0x0,0x2,&check1lock2);
	Card57_Read(0x0,0x6,&check1lock3);
	Card57_Read(0x0,0x3,&check1lock4);	 
	Card57_Read(0x0,0x1,&check2lock1);
	Card57_Read(0x0,0x2,&check2lock2);
	Card57_Read(0x0,0x6,&check2lock3);
	Card57_Read(0x0,0x3,&check2lock4);	 
	Card57_Read(0x0,0x1,&check3lock1);
	Card57_Read(0x0,0x2,&check3lock2);
	Card57_Read(0x0,0x6,&check3lock3);
	Card57_Read(0x0,0x3,&check3lock4);	 
	if (check1lock1==check2lock1)  lock1=check2lock1;
	if (check3lock1==check2lock1)  lock1=check2lock1;
	if (check3lock1==check1lock1)  lock1=check1lock1;
	if (check1lock2==check2lock2)  lock2=check2lock2;
	if (check3lock2==check2lock2)  lock2=check2lock2;
	if (check3lock2==check1lock2)  lock2=check1lock2;
	if (check1lock3==check2lock3)  lock3=check2lock3;
	if (check3lock3==check2lock3)  lock3=check2lock3;
	if (check3lock3==check1lock3)  lock3=check1lock3;  
	if (check1lock4==check2lock4)  lock4=check2lock4;
	if (check3lock4==check2lock4)  lock4=check2lock4;
	if (check3lock4==check1lock4)  lock4=check1lock4;   

	Card57Ladder->cardId[4]=(lock2>>16)&0xFF;//读卡号
	Card57Ladder->cardId[3]=(lock2>>24)&0xFF;
	Card57Ladder->cardId[2]=(lock1)&0xFF;
	Card57Ladder->cardId[1]=(lock1>>8)&0xFF;
	Card57Ladder->cardId[0]=(lock1>>16)&0xFF;
	Card57Ladder->cflag=(lock1>>24)&0xFF;//读卡标志位
	Card57Ladder->watersalesamount[0]=(lock3>>24)&0xFF;//读卡内余额
	Card57Ladder->watersalesamount[1]=(lock3>>16)&0xFF;
	Card57Ladder->watersalesamount[2]=(lock3>>8)&0xFF;
	Card57Ladder->watersalesamount[3]=(lock3)&0xFF;
	Card57Ladder->total[0]=(lock4>>16)&0xFF;
	Card57Ladder->total[1]=(lock4>>8)&0xFF;
	Card57Ladder->total[2]=(lock4)&0xFF;
	Card57_PowerOff();

 }
/*******************************************************************************
* 函 数 名         : write5557ladderCID
* 函数功能		     : 写57阶梯卡
* 输    入         : 无
* 输    出         : 1 写卡成功  0 写卡失败
*******************************************************************************/
u8 Card57_LadderWriteCID(Card57_LadderInfo Card57Ladder)
{
	Card57_PowerOn();
	if( Card57_Write(3,Card57Ladder.ladderamount[1],Card57Ladder.total[0],
	Card57Ladder.total[1],Card57Ladder.total[2])==0)
	{ 
		Card57_PowerOff();return 0;
	}//二量高ladderamount[1]
	
	if( Card57_Write(4,Card57Ladder.ladderprice[0],Card57Ladder.ladderprice[1],
	Card57Ladder.ladderprice[2],Card57Ladder.ladderprice[3])==0)
	{ 
		Card57_PowerOff();return 0;
	}//写一价二价
	
	if( Card57_Write(5,Card57Ladder.ladderprice[4],Card57Ladder.ladderprice[5],
	Card57Ladder.ladderamount[0],Card57Ladder.ladderamount[2])==0)
	{ 	
		Card57_PowerOff();return 0;
	}//写三价 一量 二量低(adderamount[2])
	
	if( Card57_Write(6,Card57Ladder.watersalesamount[0],Card57Ladder.watersalesamount[1],
	Card57Ladder.watersalesamount[2],Card57Ladder.watersalesamount[3])==0)
	{ 
		Card57_PowerOff();return 0;
	}//写本次金额
	
	Card57_PowerOff();
	return 1;
}
/******************** (C) COPYRIGHT 2021 江苏恒沁科技有限公司 ********************/
