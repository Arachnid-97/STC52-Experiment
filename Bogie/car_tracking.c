#include "car_tracking.h"
#include "bsp_time.h"
#include "car_drv.h"
#include "ultrasound_wave.h"

bit g_Left_Adjust_flag = 0;
bit g_Right_Adjust_flag = 0;

/************************************************
�������� �� Left_Dispose
��    �� �� ��ת����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Left_Dispose(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(HIGH);
}

/************************************************
�������� �� Right_Dispose
��    �� �� ��ת����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Right_Dispose(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
�������� �� Infrared_Task
��    �� �� ���⴦��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Infrared_Task(void)
{
	if(!TRACE_LEFT && TRACE_CENTRE && !TRACE_RIGHT)
	{
		/* �������� */
		g_Duty_Cycle = PWM_CLASS;
		g_Run_flag = 1;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 0;
		g_Left_Adjust_flag = 0;
		g_Right_Adjust_flag = 0;
	}
	else if(TRACE_LEFT && TRACE_CENTRE && !TRACE_RIGHT)		// ��߰ڳ�
	{
//		/* ֹͣ */
//		g_Run_flag = 0;
//		g_Stop_flag = 1;
//		g_Left_flag = 0;
//		g_Right_flag = 0;
		
		/* ��ת */
		g_Duty_Cycle = PWM_CLASS - 1;
		g_Run_flag = 0;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 1;
//		g_Left_Adjust_flag = 0;
//		g_Right_Adjust_flag = 1;
	}
	else if(!TRACE_LEFT && TRACE_CENTRE && TRACE_RIGHT)		// �ұ߰ڳ�
	{
//		/* ֹͣ */
//		g_Run_flag = 0;
//		g_Stop_flag = 1;
//		g_Left_flag = 0;
//		g_Right_flag = 0;
		
		/* ��ת */
		g_Duty_Cycle = PWM_CLASS - 1;
		g_Run_flag = 0;
		g_Stop_flag = 0;
		g_Left_flag = 1;
		g_Right_flag = 0;
//		g_Left_Adjust_flag = 1;
//		g_Right_Adjust_flag = 0;
	}
	else
	{
		/* ֹͣ */
		g_Duty_Cycle = PWM_CLASS;
		g_Run_flag = 0;
		g_Stop_flag = 1;
		g_Left_flag = 0;
		g_Right_flag = 0;
//		g_Left_Adjust_flag = 0;
//		g_Right_Adjust_flag = 0;		
	}
}


/*---------------------------- END OF FILE ----------------------------*/


