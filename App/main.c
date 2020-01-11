#include "reg52.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_exti.h"
#include "bsp_uart.h"
//#include "nixie_drv.h"
//#include "rf_send.h"
//#include "rf_receive.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"
//#include "nrf24l01_drv.h"
#include "car_drv.h"
#include "car_avoid.h"
#include "car_tracking.h"
#include "car_remote.h"


code uint32_t Baudrate = 4800;						//����������     ֧�ֵĲ����ʣ�1200,2400,4800,9600,19200,38400,57600,115200

/************************************************
�������� �� System_Start
��    �� �� ϵͳ��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void System_Start(void)
{
	clr_EA;
	
	Bsp_Init();
	UART_Timer1_Config(Baudrate);
	Time0_Config();
//	Time1_Config();
	Time2_Config();
//	Nixie_Tube_Init();
	Lcd1602_Init(ENABLE, DISABLE, DISABLE, ENABLE, DISABLE);
	UT_Config();
//	RF_Tx_Config();
//	NRF24L_Init();
	Car_Init();

	Lcd_Printf("Mode:", 0, 0);	
	Lcd_Printf("Empty", 7, 0);

	set_EA;
}


/************************************************
�������� �� main
��    �� �� ���������
��    �� �� ��
�� �� ֵ �� int
*************************************************/
int main(void)
{
	uint8_t i = 0;
//	code uint8_t buf[32] = {0x5A, 0x32, 0x05};
//	code uint8_t buf1[32] = {0x13, 'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D', '\n'};
	
	System_Start();

	Servo_Control(CENTER);
	
	SoftwareDelay_ms(100);
	
//	g_Function = 0x97;
	
	while(1)
	{
		
#if 1
		/* ģʽ */
		if(0x97 == g_Function)			// ����
		{
			if(!g_UT_Wait_flag)
			{
				UT_Send();
			}		
			if(Obstacle_Deal())
			{
				while(Servo_Deal())
				{
					if(g_Key_Down)
					{
						break;
					}
				}
			}			
		}
		else if(0x05 == g_Function)		// Ѱ��
		{
			Infrared_Deal();
		}
		else if(0x28 == g_Function)		// ң��
		{
			Control_Deal();
		}
		
		/* ����ѡ�� */
		if(KEY5 && g_Key_Down)
		{
			g_Key_time = 0;
			clr_EA;
			
			Car_Function(g_Function);			
			
			TF2 = 0;
			set_EA;

			g_Key_Down = 0;
		}
		
#endif		
		
//		/* �����շ� */
//		if(g_Frame_flag)
//		{
//			g_Frame_flag = 0;
//			UART_SendString(g_ReceiveBuf, g_RxCnt);
//			g_RxCnt = 0;
//		}
			
		/* 2.4g�շ����� */
//		if(!KEY5)
//		{
//			NRF24L_SendPacket(buf1);
//			SoftwareDelay_ms(300);
//		}
//		NRF24L_ReceivePacket(buf);
		
		/* ����ܲ��� */
//		Dig_Display(buf, 3);
//		LEVEL_COMM ^= 1;
		
//		/* ���������� */
//		UT_Ranging();
//		SoftwareDelay_ms(500);
		
//		/* Lcd���� */
//		if(!KEY5)
//		{
////			Lcd1602_Write_Cmd(LCD_CLEAR);
//			Lcd_Printf("wwe", 0, 1);
//		}

// 		/* RF���Ͳ��� */
//		if(!KEY5)
//		{
//			for(i = 0;i < 5;i++)
//			{
//				RF_Tx_Dispose(buf, 3);
//			}			
//		}

	}
}


/*---------------------------- END OF FILE ----------------------------*/


