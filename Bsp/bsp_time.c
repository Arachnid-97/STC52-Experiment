#include "bsp_time.h"
#include "bsp.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "car_avoid.h"
#include "ultrasound_wave.h"


uint32_t g_Time_1ms = 0;
uint32_t g_Time_1s = 0;
uint16_t g_Duty_Cycle = 0;			// PWMռ�ձ�

/************************************************
�������� �� SoftwareDelay_ms
��    �� �� ��� 1������ʱ
��    �� �� Count
�� �� ֵ �� ��
*************************************************/
void SoftwareDelay_ms( uint32_t Cnt )
{
    uint8_t i,j;
    while(Cnt--)
    {
        for(i = 2;i > 0;i--)
			for(j = 43;j > 0;j--)
				;
    }
}

/************************************************
�������� �� SoftwareDelay_5us
��    �� �� ��� 5΢����ʱ
��    �� �� Count
�� �� ֵ �� ��
*************************************************/
void SoftwareDelay_5us( uint32_t Cnt )
{
    uint8_t i;
    while(Cnt--)
    {
        for(i = 2;i > 0;i--);
    }
}

/************************************************
�������� �� Delay_100us
��    �� �� Timer0��ʱ����100΢��
��    �� �� CNT ---- ��ʱ����
�� �� ֵ �� ��
*************************************************/

#if 0
void Delay_100us( uint32_t Cnt )
{
	bit temp = 0;
	uint8_t h,l;
	
	h = TH0;
	l = TL0;
	temp = ET0;
	ET0 = 0;					// �ض�ʱ�� 0�ж�
	TH0 = 0xFF;
	TL0 = 0xAF;	
    COUNT_ENABLE;                        
	while (Cnt --)
	{
        TH0 = 0xFF;
        TL0 = 0xAF;
		while (!TF0);
		TF0 = 0;
	}
	COUNT_DISABLE;
	TH0 = h;
	TL0 = l;	
	ET0 = temp;	
}

#endif

/************************************************
�������� �� TIM0_GetCounter
��    �� �� ��ʱ�� 0������ȡ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
uint16_t TIM0_GetCounter(void)
{
	uint16_t tmpcnt = 0;
	uint8_t tmpcntrl, tmpcntrh;

	tmpcntrh = TH0;
	tmpcntrl = TL0;

	tmpcnt = (uint16_t)(tmpcntrl);
	tmpcnt |= (uint16_t)((uint16_t)tmpcntrh << 8);

	return ((uint16_t)tmpcnt);
}

/************************************************
�������� �� Time0_Config
��    �� �� ��ʱ�� 0��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Time0_Config(void)
{
	TMOD |= 0x01;			// ������ʽһ
	TH0 = 0;
	TL0 = 0;				// װ��ֵ
	IPH &= ~0x02;
	PT0 = 1;				// ���ȼ� 1	
//	ET0 = 1;				// ����ʱ�� 0�ж�
}

/************************************************
�������� �� Time1_Config
��    �� �� ��ʱ�� 1��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/

#if 0
void Time1_Config(void)
{
	TMOD |= 0x10;			// ������ʽһ
	TH1 = 0xFF;
	TL1 = 0x9C;				// װ��ֵ
	ET1 = 1;				// ����ʱ�� 1�ж�
	TR1 = 1;				// ����ʱ
}

#endif

/************************************************
�������� �� Time2_Config
��    �� �� ��ʱ�� 2��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Time2_Config(void)
{
	RCLK = 0;			
	TCLK = 0;
	CP_RL2 = 0;				// 16λ�Զ���װģʽ
	TH2 = 0xFC;
	TL2 = 0x18;				// װ��ֵ
	RCAP2H = 0xFC;
	RCAP2L = 0x18;			// ��װֵ(1ms)
//	RCAP2H = 0xFE;
//	RCAP2L = 0x0C;			// ��װֵ(0.5ms)	
	C_T2 = 0;				// ���ڲ���ʱ��
	IPH |= 0x20;
	PT2 = 1;				// ���ȼ� 3
	ET2 = 1;				// ����ʱ�� 2�ж�	
	TR2 = 1;				// ����ʱ
}

/************************************************
�������� �� Timer0_ISR
��    �� �� ��ʱ�� 0�ж�
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Timer0_ISR(void)	interrupt 1
{
	TF0 = 0;
	TH0 = 0xFF;
	TL0 = 0x9C;				// װ��ֵ
	
	g_Servo_Time++;
	if(g_Servo_Time <= g_Servo_Angle)
	{
		SERVO(HIGH);
	}
	else
	{
		SERVO(LOW);
	}
	if(g_Servo_Time >= 200)
	{
		g_Servo_Time = 0;
	}	
}

/************************************************
�������� �� Timer1_ISR
��    �� �� ��ʱ�� 1�ж�(PWM����)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Timer1_ISR(void)	interrupt 3
{	
	TF1 = 0;
	TH1 = 0xFF;
	TL1 = 0x9C;				// װ��ֵ	
}

/************************************************
�������� �� Timer2_ISR
��    �� �� ��ʱ�� 2�ж�(��ʱɨ��)
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Timer2_ISR(void)	interrupt 5
{
	static uint8_t PWM_cnt = 0;
	
	TF2 = 0;

	g_Time_1ms++;
	if(g_Time_1ms >= 1000)
	{
		g_Time_1ms = 0;
//		g_Time_1s++;
//		if(g_Time_1s > 1000)
//		{
//			g_Time_1s = 0;
//		}
		if(g_UT_Time) 
		{
			g_UT_Time--;
		}
		else
		{
			if(g_UT_Wait_flag)
			{
				g_UT_Wait_flag = 0;		// ��ʱ����
			}
		}
	}
	if(!KEY5 && !g_Key_Down)
	{
		g_Key_time++;
		if(g_Key_time > 30)					// ����
		{
			g_Key_time = 0;
			g_Key_Down = 1;
		}
	}
	if(g_Uart_time)
	{
		g_Uart_time--;
		if(!g_Uart_time)
		{
			g_Frame_flag = 1;
		}
	}
	
	if(PWM_cnt > g_Duty_Cycle)
	{
		Car_Control();
		if(PWM_cnt >= PWM_GRADE)
		{
			PWM_cnt = 0;
			Stop();
		}		
	}
	PWM_cnt++;
	
//	P2_1 ^= 1;	
}

/*------------------------------- END OF FILE -------------------------------*/


