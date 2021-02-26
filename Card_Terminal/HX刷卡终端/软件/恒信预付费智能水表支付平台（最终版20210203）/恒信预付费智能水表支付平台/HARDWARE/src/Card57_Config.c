/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��Card57_Config.c
 * ��Ŀ���� ��57��ͨѶ�ӿ����       
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ʱ  ��   ��2021-1-7
**********************************************************************************/
#include "Card57_Config.h"
#include "stm32f10x_exti.h"
#include "systick.h"
#include "string.h"

volatile uint32_t   Card57_Flag=0; //57���жϱ�־λ
/*******************************************************************************
* �� �� ��         : Card57_Init
* ��������		     : 57�����ų�ʼ��  
* ��    ��         : m_irq����ռ���ȼ���1����s_irq����Ӧ���ȼ���0��
* ��    ��         : ��
*******************************************************************************/
void Card57_Init(u16 m_irq, u16 s_irq)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); /* ����GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=Card57_OUT;	 //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* ��ʼ��GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_CFE;	 //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;	  //���ø�������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* ��ʼ��GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_POWER;	 //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* ��ʼ��GPIO */	
	
	GPIO_InitStructure.GPIO_Pin=Card57_ICSW ;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(Card57_GPIO,&GPIO_InitStructure); /* ��ʼ��GPIO */	
		
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);//ѡ��GPIO�ܽ������ⲿ�ж���·
	//�˴�һ��Ҫ��ס���˿ڹܽż����ж��ⲿ��·
	/* �����ⲿ�жϵ�ģʽ */ 
	EXTI_InitStructure.EXTI_Line=EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	/* ����NVIC���� */	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		 
	NVIC_InitStructure.NVIC_IRQChannel = Card57_IRQn; 	//��EXTI2��ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = m_irq; //��ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = s_irq;		  //��Ӧ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 		  //ʹ��
	NVIC_Init(&NVIC_InitStructure); 
}
/*******************************************************************************
* �� �� ��         : Card57_PowerOn
* ��������		     : ��57����Դ  
* ��    ��         : ��
* ��    ��         : ��
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
* �� �� ��         : Card57_PowerOff
* ��������		     : �ر�57����Դ  
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void Card57_PowerOff(void)
{
	Card57_POWER_L;
}
/*******************************************************************************
* �� �� ��         : delay192
* ��������		     : ��ʱ192΢�� 
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void delay192(void)
{
	delay_us(192);
}
/*******************************************************************************
* �� �� ��         : delay400
* ��������		     : ��ʱ400΢�� 
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void delay400(void)
{
	delay_us(400);
}
/*******************************************************************************
* �� �� ��         : delay448
* ��������		     : ��ʱ448΢�� 
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
static void delay448(void)
{
	delay_us(448);
}
/*******************************************************************************
* �� �� ��         : Card57_Writebit
* ��������		     : ��57����λ���в���
* ��    ��         : data:����
* ��    ��         : ��
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
* �� �� ��         : Card57_WritePassword
* ��������		     : ��57���Ľ���д�������
* ��    ��         : data:ָ���������ݵ�ָ��
* ��    ��         : ��
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
* �� �� ��         : Card57_Read
* ��������		     : ��ȡ57���е�����
* ��    ��         : page:ҳ, block1���У�data1����ֵ
* ��    ��         : ��
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
		SysTick->CTRL=0;	//�رռ�����
		SysTick->VAL=0;		//��ռ�����

		atime[j]=(count/9);

		SysTick->LOAD=9000;		 
		SysTick->CTRL=0X01;		 
		SysTick->VAL=0;	
		
	highlevel:
		outtime=0x3ffff;
		while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_CFE)==1) if(outtime--==0) goto error;
		
		count=SysTick->LOAD-SysTick->VAL;
		if(count<100) goto highlevel;
		
		SysTick->CTRL=0;	//�رռ�����
		SysTick->VAL=0;		//��ռ�����

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
* �� �� ��         : Card57_Write
* ��������		     : ��57��д������  
* ��    ��         :  block1���У�data1-4����ֵ
* ��    ��         : ����ֵ
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
	Card57_WritePassword(0x8899,0x8644);//���봦��
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
* �� �� ��         :	Card57_PreReadCID
* ��������		     :	��5557Ԥ���ѿ�ID
* ��    ��         :	��
* ��    ��         :	Card57_PreInfo
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
	Card57Pre->cflag=(lock1>>24)&0xFF;	//������־λ
	Card57Pre->watersalesamount[0]=(lock3>>24)&0xFF;
	Card57Pre->watersalesamount[1]=(lock3>>16)&0xFF;
	Card57Pre->total[0]=(lock3>>8)&0xFF;
	Card57Pre->total[1]=(lock3)&0xFF;
	Card57_PowerOff();
}
/*******************************************************************************
* �� �� ��         :  Card57_PreWrite
* ��������		     :  д5557Ԥ���ѿ�
* ��    ��         :  ��
* ��    ��         :  1 д���ɹ�  0 д��ʧ��
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
	flag2= Card57_Write(6,0x0,0x0,0x0,0x0);//20-23����
	if(flag==0||flag2==0)
	{
		return 0 ;//����ʧ��
	}
	Card57_PowerOff();
	return 1;//�����ɹ�
}
/*******************************************************************************
* �� �� ��         : Card57_LadderReadCID
* ��������		     : ��57���ݿ�id
* ��    ��         : ��
* ��    ��         : ���Ź�5���ֽ�
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

	Card57Ladder->cardId[4]=(lock2>>16)&0xFF;//������
	Card57Ladder->cardId[3]=(lock2>>24)&0xFF;
	Card57Ladder->cardId[2]=(lock1)&0xFF;
	Card57Ladder->cardId[1]=(lock1>>8)&0xFF;
	Card57Ladder->cardId[0]=(lock1>>16)&0xFF;
	Card57Ladder->cflag=(lock1>>24)&0xFF;//������־λ
	Card57Ladder->watersalesamount[0]=(lock3>>24)&0xFF;//���������
	Card57Ladder->watersalesamount[1]=(lock3>>16)&0xFF;
	Card57Ladder->watersalesamount[2]=(lock3>>8)&0xFF;
	Card57Ladder->watersalesamount[3]=(lock3)&0xFF;
	Card57Ladder->total[0]=(lock4>>16)&0xFF;
	Card57Ladder->total[1]=(lock4>>8)&0xFF;
	Card57Ladder->total[2]=(lock4)&0xFF;
	Card57_PowerOff();

 }
/*******************************************************************************
* �� �� ��         : write5557ladderCID
* ��������		     : д57���ݿ�
* ��    ��         : ��
* ��    ��         : 1 д���ɹ�  0 д��ʧ��
*******************************************************************************/
u8 Card57_LadderWriteCID(Card57_LadderInfo Card57Ladder)
{
	Card57_PowerOn();
	if( Card57_Write(3,Card57Ladder.ladderamount[1],Card57Ladder.total[0],
	Card57Ladder.total[1],Card57Ladder.total[2])==0)
	{ 
		Card57_PowerOff();return 0;
	}//������ladderamount[1]
	
	if( Card57_Write(4,Card57Ladder.ladderprice[0],Card57Ladder.ladderprice[1],
	Card57Ladder.ladderprice[2],Card57Ladder.ladderprice[3])==0)
	{ 
		Card57_PowerOff();return 0;
	}//дһ�۶���
	
	if( Card57_Write(5,Card57Ladder.ladderprice[4],Card57Ladder.ladderprice[5],
	Card57Ladder.ladderamount[0],Card57Ladder.ladderamount[2])==0)
	{ 	
		Card57_PowerOff();return 0;
	}//д���� һ�� ������(adderamount[2])
	
	if( Card57_Write(6,Card57Ladder.watersalesamount[0],Card57Ladder.watersalesamount[1],
	Card57Ladder.watersalesamount[2],Card57Ladder.watersalesamount[3])==0)
	{ 
		Card57_PowerOff();return 0;
	}//д���ν��
	
	Card57_PowerOff();
	return 1;
}
/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************/
