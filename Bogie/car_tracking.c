#include "car_tracking.h"
#include "bsp_time.h"
#include "car_drv.h"
#include "ultrasound_wave.h"

bit g_Left_Adjust_flag = 0;
bit g_Right_Adjust_flag = 0;

/************************************************
函数名称 ： Left_Dispose
功    能 ： 左转处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Left_Dispose(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(LOW);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(HIGH);
}

/************************************************
函数名称 ： Right_Dispose
功    能 ： 右转处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Right_Dispose(void)
{
	MOTOR_OUT1(HIGH);
	MOTOR_OUT2(HIGH);
	MOTOR_OUT3(HIGH);
	MOTOR_OUT4(LOW);
}

/************************************************
函数名称 ： Infrared_Task
功    能 ： 红外处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Infrared_Task(void)
{
	if(!TRACE_LEFT && TRACE_CENTRE && !TRACE_RIGHT)
	{
		/* 正常运行 */
		g_Duty_Cycle = PWM_CLASS;
		g_Run_flag = 1;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 0;
		g_Left_Adjust_flag = 0;
		g_Right_Adjust_flag = 0;
	}
	else if(TRACE_LEFT && TRACE_CENTRE && !TRACE_RIGHT)		// 左边摆出
	{
//		/* 停止 */
//		g_Run_flag = 0;
//		g_Stop_flag = 1;
//		g_Left_flag = 0;
//		g_Right_flag = 0;
		
		/* 右转 */
		g_Duty_Cycle = PWM_CLASS - 1;
		g_Run_flag = 0;
		g_Stop_flag = 0;
		g_Left_flag = 0;
		g_Right_flag = 1;
//		g_Left_Adjust_flag = 0;
//		g_Right_Adjust_flag = 1;
	}
	else if(!TRACE_LEFT && TRACE_CENTRE && TRACE_RIGHT)		// 右边摆出
	{
//		/* 停止 */
//		g_Run_flag = 0;
//		g_Stop_flag = 1;
//		g_Left_flag = 0;
//		g_Right_flag = 0;
		
		/* 左转 */
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
		/* 停止 */
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


