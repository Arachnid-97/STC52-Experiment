#include "car_avoid.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"


uint8_t g_Servo_Count = 0;		// �����������
xdata uint8_t g_Servo_Angle = CENTER;	// ����Ƕ�����
uint16_t g_Servo_Time = 0;		// �����ʱ

/************************************************
�������� �� Servo_Control
��    �� �� �ӻ�ת�ǿ���
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
void Servo_Control( uint8_t Time )
{

#if 1
	g_Servo_Count = 5;
	g_Servo_Time = 1;
	g_Servo_Angle = Time;
	
	SERVO(LOW);
	
	TF0 = 0;
	TH0 = 0xFF;
	TL0 = 0x9C;				// װ��ֵ 100us
	ET0 = 1;				// ����ʱ�� 0�ж�
	COUNT_ENABLE;
	while(g_Servo_Time)
		continue;
	TIM0_CNTR_CLEAR;
	
	SERVO(HIGH);
	
#endif
}

/************************************************
�������� �� Servo_Deal
��    �� �� �ӻ�����
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
bit Servo_Deal(void)
{
	uint8_t temp = 0;
	uint16_t l_dist,r_dist;
	static uint8_t direction = 0;	// ���Ǳ�־
			
	Servo_Control(LEFT_CORNER);		// ��߲��
	DUBUG_PRINTF(">>>>> left:");
	l_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);
	SoftwareDelay_ms(600);
	
	Servo_Control(RIGHT_CORNER);	// �ұ߲��
	
	r_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);			// �Ƕȸ�λ
	SoftwareDelay_ms(600);
	
#if 1
	g_Duty_Cycle = PWM_CLASS - 2;	// �Ӵ�Ť��

	g_Run_flag = 0;
	g_Stop_flag = 0;
	
	if(!direction)
	{
		if(l_dist >= r_dist)
		{
			/* ��ת */
			g_Left_flag = 1;
			g_Right_flag = 0;
			
			direction = 0xF0;
		}
		else
		{
			/* ��ת */
			g_Left_flag = 0;
			g_Right_flag = 1;
			
			direction = 0x0F;
		}
	}
	else if(0xF0 == direction)
	{
		/* ��ת */
		g_Left_flag = 1;
		g_Right_flag = 0;		
	}
	else if(0x0F == direction)
	{
		/* ��ת */
		g_Left_flag = 0;
		g_Right_flag = 1;		
	}
	
	SoftwareDelay_ms(300);
	g_Stop_flag = 1;
	DUBUG_PRINTF(">>>>> Current:");
	temp = UT_Ranging();
	
	if(!INFRARED_LEFT || !INFRARED_RIGHT)
	{
		/* ���� */
		g_Run_flag = 0;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 0;
		
		SoftwareDelay_ms(300);		
		g_Stop_flag = 1;					// ֹͣ
		
		temp = 1;
	}
	else
	{
		if(temp > DISTANCE_Min)
		{
			/* �������� */
			g_Run_flag = 1;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;
			
			direction = 0;					// �˳����
			temp = 0;
		}
		else
		{
			temp = 1;
		}
	}

#endif
	
	g_Duty_Cycle = PWM_CLASS;
	
	return temp;
}

/************************************************
�������� �� Obstacle_Deal
��    �� �� ���ϴ���
��    �� �� ��
�� �� ֵ �� 0 / 1
*************************************************/
bit Obstacle_Deal(void)
{
	uint8_t temp = 0;
	uint8_t UT_distance[3] = {0};

	DUBUG_PRINTF(">>>>> Distance:");
	g_UT_Distance = UT_Ranging();

	if(DISTANCE_Min < g_UT_Distance)
	{
		/* �������� */
		g_Run_flag = 1;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 0;
		
		temp = 0;
	}
	else
	{
		g_Run_flag = 0;
		
		if(g_UT_Distance <= 15)				// �������޾���
		{
			if(INFRARED_QUEEN)				// �ж�β���ϰ���
			{
				/* ���� */
				g_Stop_flag = 0;
				g_Left_flag = 0;
				g_Right_flag = 0;
				
				SoftwareDelay_ms(550);
			}
		}
		
		g_Stop_flag = 1;					// ֹͣ			
		
		temp = 1;
	}

	if(!INFRARED_LEFT || !INFRARED_RIGHT)		// ���⸨�����
	{
		if(INFRARED_QUEEN)						// �ж�β���ϰ���
		{
			/* ���� */
			g_Run_flag = 0;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;
			
			SoftwareDelay_ms(400);
		}
		g_Stop_flag = 1;
		
		temp = 1;
	}

	return temp;
}

/************************************************
�������� �� Obstacle_Task
��    �� �� ��������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Obstacle_Task(void)
{
	if(Obstacle_Deal())
	{
		while(Servo_Deal())
		{
			if(g_Key_Down)
			{
				return;
			}
		}
	}			
}


/*---------------------------- END OF FILE ----------------------------*/


