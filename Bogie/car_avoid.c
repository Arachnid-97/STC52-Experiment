#include "car_avoid.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"


uint8_t g_Servo_Count = 0;		// 舵机方波次数
xdata uint8_t g_Servo_Angle = CENTER;	// 舵机角度设置
uint16_t g_Servo_Time = 0;		// 舵机计时

/************************************************
函数名称 ： Servo_Control
功    能 ： 陀机转角控制
参    数 ： 无
返 回 值 ： 0 / 1
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
	TL0 = 0x9C;				// 装初值 100us
	ET0 = 1;				// 开定时器 0中断
	COUNT_ENABLE;
	while(g_Servo_Time)
		continue;
	TIM0_CNTR_CLEAR;
	
	SERVO(HIGH);
	
#endif
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
			
	Servo_Control(LEFT_CORNER);		// 左边测距
	DUBUG_PRINTF(">>>>> left:");
	l_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);
	SoftwareDelay_ms(600);
	
	Servo_Control(RIGHT_CORNER);	// 右边测距
	
	r_dist = UT_Ranging();
	SoftwareDelay_ms(600);
	
	Servo_Control(CENTER);			// 角度复位
	SoftwareDelay_ms(600);
	
#if 1
	g_Duty_Cycle = PWM_CLASS - 2;	// 加大扭力

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
	DUBUG_PRINTF(">>>>> Current:");
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

#endif
	
	g_Duty_Cycle = PWM_CLASS;
	
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

	DUBUG_PRINTF(">>>>> Distance:");
	g_UT_Distance = UT_Ranging();

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
		
		if(g_UT_Distance <= 15)				// 超出极限距离
		{
			if(INFRARED_QUEEN)				// 判断尾部障碍物
			{
				/* 后退 */
				g_Stop_flag = 0;
				g_Left_flag = 0;
				g_Right_flag = 0;
				
				SoftwareDelay_ms(550);
			}
		}
		
		g_Stop_flag = 1;					// 停止			
		
		temp = 1;
	}

	if(!INFRARED_LEFT || !INFRARED_RIGHT)		// 红外辅助监控
	{
		if(INFRARED_QUEEN)						// 判断尾部障碍物
		{
			/* 后退 */
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
函数名称 ： Obstacle_Task
功    能 ： 避障任务
参    数 ： 无
返 回 值 ： 无
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


