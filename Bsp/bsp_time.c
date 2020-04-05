#include "bsp_time.h"
#include "bsp.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "car_avoid.h"
#include "ultrasound_wave.h"


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
				continue;
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

	return tmpcnt;
}

/************************************************
�������� �� Time0_Config
��    �� �� ��ʱ�� 0��ʼ������������ʱ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Time0_Config(void)
{
	TMOD |= 0x01;			// ������ʽһ
	TH0 = 0;
	TL0 = 0;				// װ��ֵ
	IPH |= 0x02;
	PT0 = 0;				// ���ȼ� 2
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
��    �� �� ��ʱ�� 2��ʼ����ϵͳ��ʱɨ�裩
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void Time2_Config(void)
{
	RCLK = 0;			
	TCLK = 0;
	CP_RL2 = 0;				// 16λ�Զ���װģʽ
	C_T2 = 0;				// ���ڲ���ʱ��
	TH2 = 0xFC;
	TL2 = 0x18;				// װ��ֵ
	RCAP2H = 0xFC;
	RCAP2L = 0x18;			// ��װֵ(1ms)
//	RCAP2H = 0xF8;
//	RCAP2L = 0x30;			// ��װֵ(2ms)	
	IPH &= ~0x20;
	PT2 = 1;				// ���ȼ� 1
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
	TH0 = 0xFF;
	TL0 = 0x9C;				// װ��ֵ��100��ʱ��

	g_Servo_Time++;
	g_Servo_Time <= g_Servo_Angle ? SERVO(HIGH):SERVO(LOW);
	if(g_Servo_Time > 200)	// ��� 50HzƵ��
	{
		g_Servo_Count--;
		if(!g_Servo_Count)
		{
			ET0 = 0;			// ���ж�
			COUNT_DISABLE;
			g_Servo_Time = 0;
		}
		else
		{
			g_Servo_Time = 1;
		}
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
	
	if(g_UT_Time) 
	{
		g_UT_Time--;
	}
	if(!KEY5 && !g_Key_Down)
	{
		g_Key_time++;
		if(g_Key_time >= 20)		// ����
		{
			g_Key_time = 0;
			g_Key_Down = 1;
		}
	}
	else
	{
		g_Key_time = 0;
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
	
	TF2 = 0;				// ������������
}

/*------------------------------- END OF FILE -------------------------------*/


