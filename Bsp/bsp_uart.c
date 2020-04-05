#include "bsp_uart.h"


#define TIMES_THE_SPEED							// 波特率倍速

#define BAUDRATE	4800						// 波特率设置     支持的波特率：1200,2400,4800,9600,19200,38400,57600,115200


bit g_Frame_flag = 0;							// 一帧完成标志
xdata uint8_t g_ReceiveBuf[MAX_L] = {0};		// 串口接收暂存缓冲区
xdata uint16_t g_RxCnt = 0;						// 串口接收数据个数
uint16_t g_Uart_time = 0;						// 串口计时

/************************************************
函数名称 ： UART_Timer1_Config
功    能 ： UART配置（用定时器1溢出作为波特率时钟源）
参    数 ： 无
返 回 值 ： 无
*************************************************/
void UART_Timer1_Config(void)
{
	TMOD &= 0x0F;					// 清除定时器1模式位	
	TMOD |= 0x20;					// 模式2 --- 8位自动重装
	SM0 = 0;			
	SM1 = 1;						// 设置为工作方式 1
	REN = 1;						// 使能接收
	
#ifdef TIMES_THE_SPEED
	PCON |= 0x80;					// 波特率加倍
	TL1 = -(FOSC/12/16/BAUDRATE);	// 装初值
	
#else
	TL1 = -(FOSC/12/32/BAUDRATE);	// 装初值
	
#endif /* TIMES_THE_SPEED */
	TH1 = TL1;	
	ET1 = 0;						// 禁止定时器1中断
	IPH |= 0x10;
	PS = 1;							// 优先级 3
	ES = 1;							// 使能串口中断
	TR1 = 1;						// 开定时器 1
}

/************************************************
函数名称 ： UART_SendByte
功    能 ： 串口字符发送
参    数 ： c ---- 发送的数据
返 回 值 ： 无
*************************************************/
void UART_SendByte( uint8_t c )
{     
	SBUF = c;
    while(!TI);
	TI = 0;
}

/************************************************
函数名称 ： UART_SendString
功    能 ： 串口字符串发送
参    数 ： pData ---- 字符串
			Length ---- 长度
返 回 值 ： 无
*************************************************/
void UART_SendString( const uint8_t *pData, uint16_t Length )
{
	while(Length--)
	{
		UART_SendByte(*pData);
		pData++;
	}
}

/************************************************
函数名称 ： UART_Printf
功    能 ： 串口打印输出
参    数 ： String	---- 字符串
返 回 值 ： 无
*************************************************/
void UART_Printf( const uint8_t *String )
{
	while((*String) != '\0')
	{
		UART_SendByte(*String);
		String++;
	}
}

/************************************************
函数名称 ： UART_Receive
功    能 ： 串口字符接收
参    数 ： 无
返 回 值 ： Receive ---- 接收的数据
*************************************************/
uint8_t UART_Receive(void)
{
    uint8_t Receive = 0;
	
    while(!RI);
    Receive = SBUF;
    RI = 0;                          		
    return Receive;
}

/************************************************
函数名称 ： UART_Isr
功    能 ： 串口中断
参    数 ： 无
返 回 值 ： 无
*************************************************/
void UART_ISR(void) interrupt 4
{
	uint8_t temp = 0;
	
	if(RI)
	{
		temp = UART_Receive();
		
		if(!g_Frame_flag)					// 判断帧处理完成与否
		{
			g_ReceiveBuf[g_RxCnt] = temp;	
			g_RxCnt++;
			if(g_RxCnt >= MAX_L)
			{
				g_RxCnt = 0;				//为了防止数组溢出
			}
			g_Uart_time = 40;				//40ms(接收超时判定)
		}				
	}
}


/*---------------------------- END OF FILE ----------------------------*/


