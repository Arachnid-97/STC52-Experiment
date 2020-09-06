#include "reg52.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_exti.h"
#include "bsp_uart.h"
#include "nixie_drv.h"
#include "lcd1602_drv.h"
#include "ultrasound_wave.h"
#include "car_drv.h"
#include "car_avoid.h"
#include "car_tracking.h"
#include "car_remote.h"


/************************************************
函数名称 ： System_Start
功    能 ： 系统初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void System_Start(void)
{
	clr_EA;
	
	Bsp_Init();
	UART_Timer1_Config();
	Time0_Config();
//	Time1_Config();
	Time2_Config();
//	Nixie_Tube_Init();
	Lcd1602_Init(ENABLE, DISABLE, DISABLE, ENABLE, DISABLE);
//	UT_Config();
	Car_Init();

	Lcd_Printf("Mode:", 0, 0);	
	Lcd_Printf("Empty", 7, 0);

	DUBUG_PRINTF("Welcome main\n");
		
	set_EA;
}

/************************************************
函数名称 ： main
功    能 ： 主函数入口
参    数 ： 无
返 回 值 ： int
*************************************************/
int main(void)
{
	uint8_t i = 0;
	uint8_t UT_distance[3] = {0};
	
	System_Start();
	
	Servo_Control(CENTER);		// 陀机矫正

	while(1)
	{
		
#if 1
		
	#if 1
		/* 模式 */
		switch(g_Function)
		{
			case 0x97:
					Obstacle_Task();
					break;
						
			case 0x05:
					Infrared_Task();
					break;
			
			case 0x28:
					Control_Task();
					break;
			
			default:
//					g_Function = 0x00;
					break;
		}

	#endif
		
		/* 功能选择 */
		if(KEY5 && g_Key_Down)
		{
			Car_Function(g_Function);
//			UART_Printf("Down\n");
			g_Key_Down = 0;
		}
		
#endif
		
	}
}


/*---------------------------- END OF FILE ----------------------------*/


