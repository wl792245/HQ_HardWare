/******************** (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ ********************
 * �ļ���   ��main.c
 * ��Ŀ���� ������ˢ���ն��豸    
 * ����ƽ̨ ������STM32F103ZET6оƬ����
 * ��  ��   ��Terry  QQ: 792245610
 * ��  ��   ��V2.0
 * ʱ  ��   ��2020-12-31
**********************************************************************************/
#include "BSP_Config.h"  



Card24_LadderInfo   Info24ladder;      //24��������Ϣ�ṹ��
Card24_PreInfo      Info24pre;         //24��Ԥ������Ϣ�ṹ��
Card42_PreInfo      Info42pre;         //42��Ԥ������Ϣ�ṹ��
Card57_PreInfo      Info57pre;
Card57_LadderInfo   Info57ladder;	

/*******************************************************************************
* �� �� ��         : main
* ��������		     : ���������������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main(void)
{

	
	BSP_Init();

	while(1)	
	{
		//24.42������
		if(CardIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			sec = 0;
			CardIsInter = 0;
			Card24_LadderReadCID(&Info24ladder);   	//��24���ݿ�
			Card42_PreReadCID(&Info42pre);			    //��4442Ԥ���ѿ�
			HDMIShowWait();                         //Һ����ʾ�����Ժ�...x%��
			if(Info42pre.cflag == 0xB2)             //4442Ԥ���ѿ�
			{
					Card42_PDeal(&Info42pre);		
			}
			else if(Info24ladder.cflag == 0xC1)      //�豸���޸Ŀ�
			{
					if(CopyCIDTo24C02())                  //�����ÿ��Ļ�����IICID���Ƶ��������ϵ�24c02
					{
						XFS_HDMI_Play("�豸�Ÿ���ʧ�ܣ�������");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("�豸�Ÿ��ĳɹ�");	
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag == 0xC2)     //��Ļ�汾���޸Ŀ�
			{
					if(CopyScrrenvTo24C02())                 //�����ÿ�����Ļ�汾�Ÿ��Ƶ��������ϵ�24c02
					{
						XFS_HDMI_Play("��Ļ�汾�Ÿ���ʧ�ܣ�������");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("��Ļ�汾�Ÿ��ĳɹ�");
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag == 0xC3)     //�豸����汾���޸Ŀ�
			{
					if (CopyTerminalVTo24C02())               //�����ÿ����豸�汾�Ÿ��Ƶ��������ϵ�24c02
					{
						XFS_HDMI_Play("�豸�汾�Ÿ���ʧ�ܣ�������");
					}
					else
					{
						HDMIShowChangeSuccess();
						XFS_Only_Play("�豸�汾�Ÿ��ĳɹ�");
						AT24C02IPTid_Init();
					}
			}
			else if(Info24ladder.cflag==0xC5)      //��ʾ��
			{
					HDMIShowServerInfo(&DeviceInfo);
					delay_ms(941);
			}
			else if ( Info24ladder.areacode[0]==0&&Info24ladder.areacode[1]==0&&Info24ladder.areacode[2]==0) // 24Ԥ���ѿ�
			{
					Card24_PreReadCID(&Info24pre);       //�ȶ�24��
					if ((Info24pre.watersalesamount[0]==0)&&(Info24pre.watersalesamount[1]==0)) //24Ԥ���ѿ���ˮλΪ0�ſ���ˢ������Ϊ�����Ȳ�ˮ��
					{
						Card24_PDeal(&Info24pre);
					}
					else
					{
						XFS_HDMI_Play("��������Ϊ�㣬�Ȳ��");
					}
					
			}
			else if ((Info24ladder.areacode[0]!=0||Info24ladder.areacode[1]!=0||
					Info24ladder.areacode[2]!=0)&&(Info24ladder.areacode[0]!=0xFF||
					Info24ladder.areacode[1]!=0xFF||Info24ladder.areacode[2]!=0xFF))//24���ݿ�
			{
				Card24_LDeal(&Info24ladder);
			}
			else
			{
				XFS_HDMI_Play("����ˢ");
			}
			while(GPIO_ReadInputDataBit(Card_GPIO,Card_ICSW)==0)	//���������ߣ��ȴ�
			{
				;
			}
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
			HDMIShowMenuInfo(&DeviceInfo);
		}
		//57������
		if (Card57_Flag == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			Card57_Flag = 0;
			sec = 0;
			Card57_PreReadCID(&Info57pre);        //��57Ԥ���ѿ�
			HDMIShowWait();                         //Һ����ʾ�����Ժ�...x%��
			if (Info57pre.cflag == 0xB4)           //5557��ΪԤ���ѿ��Ĵ���
			{
				Card57_PDeal(&Info57pre);
			}
			else if (Info57pre.cflag == 0xB6)     ////���ݱ���
			{
				Card57_LadderReadCID(&Info57ladder);       //��57���ݿ�
				Card57_LDeal(&Info57ladder);
			}
			else
			{
				XFS_HDMI_Play("����ˢ");
			}
		  while(GPIO_ReadInputDataBit(Card57_GPIO,Card57_ICSW)==0)	//���������ߣ��ȴ�
			{
				;
			}
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
			HDMIShowMenuInfo(&DeviceInfo);
		}	
    //��ʱ������
		if(TimeIsInter == 1)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			TimeIsInter = 0;
			TIM_Heart_Deal();
			HDMIShowMenuInfo(&DeviceInfo);
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
		}
		//����M6312
    if (M6311_RestartFlag == 0x63)
		{
			TIM_Cmd(TIM3,DISABLE); //ʹ�ܻ���ʧ��TIMx����
			M6311_RestartFlag = 0;
			sec = 0;
			M6311_Connect(&DeviceInfo, "����������...");  //M6311���ӷ�����
			FirstHeart_Open(DeviceInfo.TerminalInfo.TerminalId);
			HDMIShowMenuInfo(&DeviceInfo);
			TIM_Cmd(TIM3,ENABLE); //ʹ�ܻ���ʧ��TIMx����
		}
	}
}

/******************* (C) COPYRIGHT 2020 ���պ��߿Ƽ����޹�˾ *****END OF FILE****/
