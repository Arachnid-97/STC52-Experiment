#include "car_avoid.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"


//#define _UART_DEBUG

uint8_t g_Servo_Angle = CENTER;	// 舵机角度设置
uint16_t g_Servo_Time = 0;		// 舵机计时

/************************************************
函数名称 ： Servo_Control
功    能 ： 陀机转角控制
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
void Servo_Control( uint8_t Time )
{
	uint8_t h,l,temp;
	
	g_Servo_Time = 0;
	g_Servo_Angle = Time;
	
	h = TH0;
	l = TL0;
	temp = ET0;	
	TF0 = 0;
	TH0 = 0xFF;
	TL0 = 0x9C;				// 装初值	
	ET0 = 1;				// 开定时器 0中断
	COUNT_ENABLE;
	SoftwareDelay_ms(80);
	COUNT_DISABLE;
	TIM0_CNTR_CLEAR;
	TF0 = 0;
	TH0 = h;
	TL0 = l;
//	ET0 = 0;
	ET0 = temp;
}

/************************************************
函数名称 ： Servo_Deal
功    能 ： 陀机处理
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
bit Servo_Deal(void)
{
	uint8_t temp = 0;
	uint16_t l_dist,r_dist;
	static uint8_t direction = 0;	// 死角标志
	
	EX1 = 0;				// 失能外部中断 1
	g_Duty_Cycle = 4;		// 加大扭力
	
	Servo_Control(LEFT_CORNER);		// 左边测距
//	UART_Printf("left:");
	l_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);
	SoftwareDelay_ms(600);
	
	Servo_Control(RIGHT_CORNER);	// 右边测距
//	UART_Printf("right:");
	r_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);			// 角度复位

	g_Run_flag = 0;
	g_Stop_flag = 0;

	if(!direction)
	{
		if(l_dist >= r_dist)
		{
			/* 左转 */
			g_Left_flag = 1;
			g_Right_flag = 0;
			
			direction = 0xF0;
		}
		else
		{
			/* 右转 */
			g_Left_flag = 0;
			g_Right_flag = 1;
			
			direction = 0x0F;
		}
	}
	else if(0xF0 == direction)
	{
		/* 左转 */
		g_Left_flag = 1;
		g_Right_flag = 0;		
	}
	else if(0x0F == direction)
	{
		/* 右转 */
		g_Left_flag = 0;
		g_Right_flag = 1;		
	}
	
	SoftwareDelay_ms(300);
	g_Stop_flag = 1;
	Servo_Control(CENTER);
	temp = UT_Ranging();
	
	if(!INFRARED_LEFT || !INFRARED_RIGHT)
	{
		/* 后退 */
		g_Run_flag = 0;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 0;
		
		SoftwareDelay_ms(300);		
		g_Stop_flag = 1;					// 停止
		
		temp = 1;
	}
	else
	{
		if(temp > DISTANCE_Min)
		{
			/* 正常运行 */
			g_Run_flag = 1;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;
			
			direction = 0;					// 退出清除
			temp = 0;
		}
		else
		{
			temp = 1;
		}
	}
		
	g_Duty_Cycle = PWM_CLASS;
	EX1 = 1;				// 使能外部中断 1	
	
	return temp;
}

/************************************************
函数名称 ： Obstacle_Deal
功    能 ： 避障处理
参    数 ： 无
返 回 值 ： 0 / 1
*************************************************/
bit Obstacle_Deal(void)
{
	uint8_t temp = 0;
	uint8_t UT_distance[3] = {0};
	
	if(g_UT_Receive_flag)
	{
		g_UT_Receive_flag = 0;
		
		g_UT_Distance = g_UT_Distance*(340L >> 1) / 10000;	// 距离	

		if(DISTANCE_Min < g_UT_Distance)
		{
			/* 正常运行 */
			g_Run_flag = 1;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;
			
			temp = 0;
		}
		else
		{
			g_Run_flag = 0;
			
			if(g_UT_Distance < 15)						// 超出极限距离
			{
				if(!INFRARED_QUEEN)
				{
					/* 后退 */
					g_Stop_flag = 0;
					g_Left_flag = 0;
					g_Right_flag = 0;
					
					SoftwareDelay_ms(300);
				}
			}
			
			g_Stop_flag = 1;					// 停止			
			
			temp = 1;
		}
		
		/* LCD处理显示 */
		g_UT_Buf[0] = (g_UT_Distance / 100) % 10;	// m
		g_UT_Buf[1] = (g_UT_Distance / 10) % 10;	// dm
		g_UT_Buf[2] = (g_UT_Distance) % 10;			// cm	

		UT_distance[0] = g_UT_Buf[0] + '0';	
		UT_distance[1] = g_UT_Buf[1] + '0';			
		UT_distance[2] = g_UT_Buf[2] + '0';				

#ifdef _UART_DEBUG						
		UART_Printf("dist:");
		UART_SendString(UT_distance, 3);	
		UART_Printf("cm");
		UART_SendByte(0x0D);		// '\r'
		UART_SendByte(0x0A);		// '\n'
	
#endif /* _UART_DEBUG */
		
		Lcd_Show_Str(UT_distance, 10, 1, 3);
	}
	
	if(!INFRARED_LEFT || !INFRARED_RIGHT)		// 红外辅助监控
	{
		if(!INFRARED_QUEEN)
		{
			/* 后退 */
			g_Run_flag = 0;
			g_Stop_flag = 0;
			g_Left_flag = 0;
			g_Right_flag = 0;
			
			SoftwareDelay_ms(650);
		}
		g_Stop_flag = 1;
		
		temp = 1;
	}
	
	return temp;
}


/*---------------------------- END OF FILE ----------------------------*/


