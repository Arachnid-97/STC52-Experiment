#ifndef __BSP_UART_H
#define __BSP_UART_H


#include "reg52.h"

#define FOSC	12000000L     	 //System frequency	（L 为长整型数据类型）
#define MAX_L	100				 //接收缓冲区最大长度

extern bit g_Receiving_flag;							
extern bit g_Frame_flag;
extern uint8_t g_ReceiveBuf[MAX_L];
extern uint16_t g_RxCnt;
extern uint16_t g_Uart_time;

void UART_Timer1_Config( uint32_t Baudrate );
void UART_SendByte( uint8_t c );
void UART_SendString( const uint8_t *pData, uint16_t Length );
void UART_Printf( const uint8_t *String );
uint8_t UART_Receive(void);


#endif /* __BSP_UART_H */


/*------------------------------- END OF FILE -------------------------------*/


