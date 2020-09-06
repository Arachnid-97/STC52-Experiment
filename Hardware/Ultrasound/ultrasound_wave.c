#include "ultrasound_wave.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "lcd1602_drv.h"


uint8_t g_UT_Time = 0;					// 超声波时间
xdata uint8_t g_UT_Buf[3] = {0};		// 超声波数据
xdata uint16_t g_UT_Distance = 0x00;	// 超声波测距距离

/************************************************
函数名称 ： UT_Ranging
功    能 ： 超声波测距(一体化,非中断测量)
参    数 ： 无
返 回 值 ： distance ---- 距离（cm）
*************************************************/

#if 1
uint16_t UT_Ranging(void)
{
	uint8_t UT_distance[3] = {0};
	uint32_t distance = 0;

    TRIG(HIGH);
    SoftwareDelay_5us(5);
    TRIG(LOW);
	
	g_UT_Time = UT_WAIT_TIME;
	
	while(!ECHO)
	{
		if(!g_UT_Time)
			break;
	}
	
	g_UT_Time = UT_WAIT_TIME;
	TIM0_CNTR_CLEAR;
	COUNT_ENABLE;
	
	while(ECHO)
	{
		if(!g_UT_Time)
			break;
	}
	COUNT_DISABLE;
	distance = TIM0_GetCounter();	// 获取时间（微秒）
	TIM0_CNTR_CLEAR;
		
	distance = distance*(340L >> 1) / 10000;	// 换算成 cm
		
	g_UT_Buf[0] = (distance / 100) % 10;	// m
	g_UT_Buf[1] = (distance / 10) % 10;		// dm
	g_UT_Buf[2] = (distance) % 10;			// cm	
	
	UT_distance[0] = g_UT_Buf[0] + '0';	
	UT_distance[1] = g_UT_Buf[1] + '0';
	UT_distance[2] = g_UT_Buf[2] + '0';				
	
//	Dig_Display(g_UT_Buf, 3);

	DUBUG_STRING(UT_distance, 3);
	
	DUBUG_PRINTF("cm\r\n");
	
	Lcd_Show_Str(UT_distance, 10, 1, 3);
	
	return distance;
}

#endif

/************************************************
函数名称 ： UT_Config
功    能 ： 超声波配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void UT_Config(void)
{
	IT1 = 1;		// 下降沿
	IPH |= 0x04;
	PX1 = 1;		// 优先级 3
	EX1 = 1;		// 使能外部中断 1
}


/*------------------------------- END OF FILE -------------------------------*/


