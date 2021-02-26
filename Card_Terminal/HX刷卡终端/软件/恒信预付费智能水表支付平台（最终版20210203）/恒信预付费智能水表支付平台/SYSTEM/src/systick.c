#include "systick.h"

static u8  fac_us=0;//us��ʱ������
static u16 fac_ms=0;//ms��ʱ������

/*******************************************************************************
* �� �� ��         : delay_init
* ��������		     : ��ʱ��ʼ��
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_init(void)
{

	#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
		u32 reload;
	#endif
		SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
		fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
		 
	#ifdef OS_CRITICAL_METHOD 	//���OS_CRITICAL_METHOD������,˵��ʹ��ucosII��.
		reload=SystemCoreClock/8000000;		//ÿ���ӵļ������� ��λΪK	   
		reload*=1000000/OS_TICKS_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ��
								//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
		fac_ms=1000/OS_TICKS_PER_SEC;//����ucos������ʱ�����ٵ�λ	   
		SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
		SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    
	#else
		fac_ms=(u16)fac_us*1000;//��ucos��,����ÿ��ms��Ҫ��systickʱ����   
	#endif
}						
/*******************************************************************************
* �� �� ��         : delay_us
* ��������		     : ��ʱ��������ʱus
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //ʱ�����	  		 
	SysTick->VAL=0x00;        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��			//bit16��һ˵��ʱ�䵽������ѭ��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	 
}

/*******************************************************************************
* �� �� ��         : delay_ms
* ��������		     : ��ʱ��������ʱms
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_ms(u16 nms)
{	 		 
	u32 temp;	
	//IWDG_Feed();	//4sιһ�ο��Ź�
	
	SysTick->LOAD=(u32)nms*fac_ms;//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;           //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;       //��ռ�����	  	    
} 

/*******************************************************************************
* �� �� ��         : CPU_Reset
* ��������		     : CPU��λ
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void	CPU_Reset(void)
{
	 //��AT24C02д��������
		__set_FAULTMASK(1);      // �ر�ϵͳ�ж�  
		/* system soft reset */
		NVIC_SystemReset();		// ������core_m3.c��
}
