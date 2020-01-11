#include "car_remote.h"
#include "bsp_uart.h"
#include "bsp_time.h"
#include "car_drv.h"
#include "ultrasound_wave.h"
#include "lcd1602_drv.h"


/************************************************
�������� �� Data_Analyze
��    �� �� ���ݴ������
��    �� �� Data ---- ����
�� �� ֵ �� ��
*************************************************/
static void Data_Analyze( uint8_t *Data )
{
	g_Duty_Cycle = PWM_GRADE - *(Data + 3);		// ����
	
	switch(*(Data + 2))
	{
		case 0xF0:
			/* ǰ�� */
			g_Duty_Cycle = PWM_CLASS;
			g_Run_flag = 1;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;			
			break;
		
		case 0x0F:
			/* ���� */
			g_Duty_Cycle = PWM_CLASS;
			g_Run_flag = 0;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;			
			break;
		
		case 0xAA:
			/* ��ת */
			g_Duty_Cycle = PWM_CLASS - 1;		// �Ӵ�Ť��
			g_Run_flag = 0;
			g_Stop_flag = 0;
			g_Left_flag = 1;
			g_Right_flag = 0;					
			break;
		
		case 0x55:
			/* ��ת */
			g_Duty_Cycle = PWM_CLASS - 1;		// �Ӵ�Ť��
			g_Run_flag = 0;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 1;			
			break;
		
		default:
			/* ֹͣ */
			g_Duty_Cycle = PWM_CLASS;
			g_Run_flag = 0;
			g_Stop_flag = 1;
			g_Left_flag = 0;
			g_Right_flag = 0;			
			break;
	}
}

/************************************************
�������� �� Data_Verify
��    �� �� У��
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
static bit Data_Verify( uint8_t *Buf )
{
	uint8_t i;
	uint16_t temp = 0;
	
	for(i = 1;i < 5;i++)
	{
		temp += *(Buf + i);
	}
	temp = (uint8_t)(~temp);
	
	if(temp == *(Buf + i))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/************************************************
�������� �� Control_Deal
��    �� �� Զ�̿��ƴ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Control_Deal(void)
{
	if(g_Frame_flag)
	{
//		UART_SendString(g_ReceiveBuf, g_RxCnt);
		
		if(6 == g_RxCnt && 0x5A == g_ReceiveBuf[0])		// Э���ж�
		{
			if(Data_Verify(g_ReceiveBuf))
			{
				switch(g_ReceiveBuf[1])
				{
					case 0x00:
						g_Function = 0x00;
						Lcd_Printf("Mode:", 0, 0);	
						Lcd_Printf("Empty", 7, 0);

					case 0x97:
						Car_Function(0x28);
						break;
					
					case 0x05:
						Car_Function(0x97);
						break;
					
					case 0x28:
						Data_Analyze(&g_ReceiveBuf);
						break;
					
					default:
						break;
				}
			}
			g_RxCnt = 0;
		}
		g_Frame_flag = 0;
	}
}


/*---------------------------- END OF FILE ----------------------------*/


