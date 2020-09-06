#include "car_drv.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "car_avoid.h"
#include "car_tracking.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"


bit g_Run_flag = 0;				// ���б�־
bit g_Stop_flag = 1;			// ֹͣ��־
bit g_Left_flag = 0;			// ��ת��־
bit g_Right_flag = 0;			// ��ת��־
uint8_t g_Function = 0x00;		// ���ܼ�

/************************************************
�������� �� Run
��    �� �� ǰ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Run(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
�������� �� BackRun
��    �� �� ����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void BackRun(void)
{
	MOTOR_OUT1(LOW);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(LOW);
	MOTOR_OUT4(HIGH);
}

/************************************************
�������� �� LeftRun
��    �� �� ��ת
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void LeftRun(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(LOW);
	MOTOR_OUT4(HIGH);
}

/************************************************
�������� �� RightRun
��    �� �� ��ת
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RightRun(void)
{
	MOTOR_OUT1(LOW);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
�������� �� Stop
��    �� �� ֹͣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Stop(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(HIGH);
}

/************************************************
�������� �� Car_Control
��    �� �� С������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Car_Control(void)
{
	if(g_Run_flag)
	{
		Run();
	}
	else
	{
		if(g_Stop_flag)
		{
			Stop();
		}
		else
		{
			if(!g_Left_flag && !g_Right_flag)
			{
				if(g_Left_Adjust_flag)
				{
					Left_Dispose();
				}
				else if(g_Right_Adjust_flag)
				{
					Right_Dispose();
				}
				else
				{
					BackRun();
				}
			}
			else if(g_Left_flag)
			{
				LeftRun();
			}
			else if(g_Right_flag)
			{			
				RightRun();
			}
		}
	}
}

/************************************************
�������� �� Car_Function
��    �� �� С������ѡ��
��    �� �� Mode ---- ����
�� �� ֵ �� ��
*************************************************/
void Car_Function( uint16_t Mode )
{
	Servo_Control(CENTER);			// �ӻ�����
	Lcd1602_Write_Cmd(LCD_CLEAR);   // ���� 
	Lcd_Printf("Mode:", 0, 0);
	
	/* ֹͣ */
	g_Run_flag = 0;
	g_Stop_flag = 1;
	g_Left_flag = 0;
	g_Right_flag = 0;
	g_Left_Adjust_flag = 0;
	g_Right_Adjust_flag = 0;
		
	switch(Mode)
	{
		case 0x00:
				Lcd_Printf("Avoid", 7, 0);
				Lcd_Printf("Distance:", 0, 1);
				Lcd_Printf("cm", 14, 1);
		
				g_Function = 0x97;				// �������� -- �������ģʽ
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM����
				g_UT_Distance = 0xFF;				
				ES = 0;							// ʧ�ܴ����жϣ�û�õ���
				break;

		case 0x97:
				Lcd_Printf("Tracking", 7, 0);
				g_Function = 0x05;				// ����Ѱ��ģʽ
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM����
				ES = 0;							// ʧ�ܴ����жϣ�û�õ���				
				break;
		
		case 0x05:
				Lcd_Printf("Remote", 7, 0);
				g_Function = 0x28;				// ����Զ�̿���ģʽ
				g_Duty_Cycle = PWM_CLASS - 2;	// PWM����
				g_RxCnt = 0;					// ������������
				ES = 1;							// ʹ�ܴ����ж�
				break;
		
		case 0x28:
				Lcd_Printf("Avoid", 7, 0);
				Lcd_Printf("Distance:", 0, 1);
				Lcd_Printf("cm", 14, 1);				
				g_Function = 0x97;				// �ص�����ģʽ
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM����
				ES = 0;							// ʧ�ܴ����жϣ�û�õ���
				break;
		
		default:
				/* ���� */
				Lcd1602_Write_Cmd(LCD_CLEAR);   //����  
				Lcd_Printf("Error", 7, 0);		
				break;
	}
}

/************************************************
�������� �� Car_Init
��    �� �� С����ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Car_Init(void)
{
	g_Run_flag = 0;
	g_Stop_flag = 1;			// ֹͣ����
	g_Duty_Cycle = PWM_CLASS;	// PWM���أ���ֵԽ���ٶ�Խ��������ʱ����ʵ�֣�
	
	g_Function = 0x00;
}


/*---------------------------- END OF FILE ----------------------------*/


