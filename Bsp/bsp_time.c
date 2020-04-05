#include "bsp_time.h"
#include "bsp.h"
#include "bsp_uart.h"
#include "car_drv.h"
#include "car_avoid.h"
#include "ultrasound_wave.h"


uint16_t g_Duty_Cycle = 0;			// PWM占空比

/************************************************
函数名称 ： SoftwareDelay_ms
功    能 ： 软件 1毫秒延时
参    数 ： Count
返 回 值 ： 无
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
函数名称 ： SoftwareDelay_5us
功    能 ： 软件 5微秒延时
参    数 ： Count
返 回 值 ： 无
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
函数名称 ： Delay_100us
功    能 ： Timer0延时——100微秒
参    数 ： CNT ---- 延时次数
返 回 值 ： 无
*************************************************/

#if 0
void Delay_100us( uint32_t Cnt )
{
	bit temp = 0;
	uint8_t h,l;
	
	h = TH0;
	l = TL0;
	temp = ET0;
	ET0 = 0;					// 关定时器 0中断
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
函数名称 ： TIM0_GetCounter
功    能 ： 定时器 0计数获取
参    数 ： 无
返 回 值 ： 无
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
函数名称 ： Time0_Config
功    能 ： 定时器 0初始化（超声波计时）
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Time0_Config(void)
{
	TMOD |= 0x01;			// 工作方式一
	TH0 = 0;
	TL0 = 0;				// 装初值
	IPH |= 0x02;
	PT0 = 0;				// 优先级 2
}

/************************************************
函数名称 ： Time1_Config
功    能 ： 定时器 1初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/

#if 0
void Time1_Config(void)
{
	TMOD |= 0x10;			// 工作方式一
	TH1 = 0xFF;
	TL1 = 0x9C;				// 装初值
	ET1 = 1;				// 开定时器 1中断
	TR1 = 1;				// 开定时
}

#endif

/************************************************
函数名称 ： Time2_Config
功    能 ： 定时器 2初始化（系统定时扫描）
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Time2_Config(void)
{
	RCLK = 0;			
	TCLK = 0;
	CP_RL2 = 0;				// 16位自动重装模式
	C_T2 = 0;				// 作内部定时器
	TH2 = 0xFC;
	TL2 = 0x18;				// 装初值
	RCAP2H = 0xFC;
	RCAP2L = 0x18;			// 重装值(1ms)
//	RCAP2H = 0xF8;
//	RCAP2L = 0x30;			// 重装值(2ms)	
	IPH &= ~0x20;
	PT2 = 1;				// 优先级 1
	ET2 = 1;				// 开定时器 2中断	
	TR2 = 1;				// 开定时
}

/************************************************
函数名称 ： Timer0_ISR
功    能 ： 定时器 0中断
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Timer0_ISR(void)	interrupt 1
{
	TH0 = 0xFF;
	TL0 = 0x9C;				// 装初值，100计时量

	g_Servo_Time++;
	g_Servo_Time <= g_Servo_Angle ? SERVO(HIGH):SERVO(LOW);
	if(g_Servo_Time > 200)	// 舵机 50Hz频率
	{
		g_Servo_Count--;
		if(!g_Servo_Count)
		{
			ET0 = 0;			// 关中断
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
函数名称 ： Timer1_ISR
功    能 ： 定时器 1中断(PWM控制)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void Timer1_ISR(void)	interrupt 3
{	
	TH1 = 0xFF;
	TL1 = 0x9C;				// 装初值	
}

/************************************************
函数名称 ： Timer2_ISR
功    能 ： 定时器 2中断(定时扫描)
参    数 ： 无
返 回 值 ： 无
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
		if(g_Key_time >= 20)		// 消抖
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
	
	TF2 = 0;				// 必须由软件清除
}

/*------------------------------- END OF FILE -------------------------------*/


