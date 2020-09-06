#include "car_drv.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "car_avoid.h"
#include "car_tracking.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"


bit g_Run_flag = 0;				// 运行标志
bit g_Stop_flag = 1;			// 停止标志
bit g_Left_flag = 0;			// 左转标志
bit g_Right_flag = 0;			// 右转标志
uint8_t g_Function = 0x00;		// 功能键

/************************************************
函数名称 ： Run
功    能 ： 前进
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Run(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
函数名称 ： BackRun
功    能 ： 后退
参    数 ： 无
返 回 值 ： 无
*************************************************/
void BackRun(void)
{
	MOTOR_OUT1(LOW);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(LOW);
	MOTOR_OUT4(HIGH);
}

/************************************************
函数名称 ： LeftRun
功    能 ： 左转
参    数 ： 无
返 回 值 ： 无
*************************************************/
void LeftRun(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(LOW);
	MOTOR_OUT4(HIGH);
}

/************************************************
函数名称 ： RightRun
功    能 ： 右转
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RightRun(void)
{
	MOTOR_OUT1(LOW);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
函数名称 ： Stop
功    能 ： 停止
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Stop(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(HIGH);
}

/************************************************
函数名称 ： Car_Control
功    能 ： 小车控制
参    数 ： 无
返 回 值 ： 无
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
函数名称 ： Car_Function
功    能 ： 小车功能选择
参    数 ： Mode ---- 功能
返 回 值 ： 无
*************************************************/
void Car_Function( uint16_t Mode )
{
	Servo_Control(CENTER);			// 陀机矫正
	Lcd1602_Write_Cmd(LCD_CLEAR);   // 清屏 
	Lcd_Printf("Mode:", 0, 0);
	
	/* 停止 */
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
		
				g_Function = 0x97;				// 初次启动 -- 进入避障模式
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM调控
				g_UT_Distance = 0xFF;				
				ES = 0;							// 失能串口中断（没用到）
				break;

		case 0x97:
				Lcd_Printf("Tracking", 7, 0);
				g_Function = 0x05;				// 进入寻迹模式
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM调控
				ES = 0;							// 失能串口中断（没用到）				
				break;
		
		case 0x05:
				Lcd_Printf("Remote", 7, 0);
				g_Function = 0x28;				// 进入远程控制模式
				g_Duty_Cycle = PWM_CLASS - 2;	// PWM调控
				g_RxCnt = 0;					// 串口数据清零
				ES = 1;							// 使能串口中断
				break;
		
		case 0x28:
				Lcd_Printf("Avoid", 7, 0);
				Lcd_Printf("Distance:", 0, 1);
				Lcd_Printf("cm", 14, 1);				
				g_Function = 0x97;				// 回到避障模式
				g_Duty_Cycle = PWM_CLASS + 0;	// PWM调控
				ES = 0;							// 失能串口中断（没用到）
				break;
		
		default:
				/* 错误 */
				Lcd1602_Write_Cmd(LCD_CLEAR);   //清屏  
				Lcd_Printf("Error", 7, 0);		
				break;
	}
}

/************************************************
函数名称 ： Car_Init
功    能 ： 小车初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Car_Init(void)
{
	g_Run_flag = 0;
	g_Stop_flag = 1;			// 停止运行
	g_Duty_Cycle = PWM_CLASS;	// PWM调控（数值越大速度越慢，看定时器的实现）
	
	g_Function = 0x00;
}


/*---------------------------- END OF FILE ----------------------------*/


