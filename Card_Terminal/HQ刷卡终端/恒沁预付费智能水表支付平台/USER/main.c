/******************** (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��main.c
 * ��Ŀ���� �����߻���ˢ���ն��豸    
 * ����ƽ̨ ������STM32F103CBT6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ��  ��   ��V2.0
 * ʱ  ��   ��2021-01-25
**********************************************************************************/
#include "BSP_Config.h"
#include "stm32f10x_tim.h"
#include "string.h"

/*******************************************************************************
* �� �� ��         : main
* ��������		     : ���������������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main(void)
{
  u8 i;
	BSP_Init();
	while(1)
	{
		Remote_ChangeIp();  //Զ��IP��ά�����
		//����������
		if(SCCardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			TIM_Cmd(TIM2,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			Server_Time = 0;
			PC_Time = 0;
			SCCardIsInter = 0;
			HDMIShowWait();
			SCCard_Deal();
		  while(GPIO_ReadInputDataBit(SCCARD_INIT_Port,SCCARD_INIT_CSW)==0)	//���������ߣ��ȴ�
			{
				;
			}
			HDMIShowMenuInfo(&DeviceInfo);        //��������ʾ
			TIM_Cmd(TIM2,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
		}
		//��ʱ������������
		if(Server_TimeIsInter == 1)
		{
			Server_TimeIsInter = 0;
			
		}
		//��ʱ�����Դ���
		if(PC_TimeIsInter == 1)
		{
			PC_TimeIsInter = 0;
		}
		//������������
		if (PC_RestartFlag == 0x86)
		{
			TIM_Cmd(TIM2,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			PC_Time = 0;
			PC_RestartFlag = 0;
			PC_Close();
			delay_ms(941);
      PC_Open();                         //΢���Կ���
			for(i=0;i<180;i++)
			{
				delay_ms(941);
			}
			if(PC_Start())
			{
				HDMIShowMenuInfo(&DeviceInfo);     //��������ʾ
				TIM_Cmd(TIM2,ENABLE); //ʹ�ܻ���ʧ��TIMx����
			}
		}
		//M6312����
		if (M6312_RestartFlag == 0x63)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			Server_Time = 0;
			M6312_RestartFlag = 0;
			M6312_Connect(&DeviceInfo, "����������...");  //M6312���ӷ�����,�ź�ǿ�ȼ�⣬ʱ���ȡ
			FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId); //��һ������
			HDMIShowMenuInfo(&DeviceInfo);        //��������ʾ
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����,��������ʱ�����¿�ʼ��ʱ
		}
	}
}





/******************* (C) COPYRIGHT 2021 ���պ��߿Ƽ����޹�˾ *****END OF FILE****/
