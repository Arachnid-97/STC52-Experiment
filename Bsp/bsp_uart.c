#include "bsp_uart.h"


#define TIMES_THE_SPEED							// �����ʱ���

#define BAUDRATE	4800						// ����������     ֧�ֵĲ����ʣ�1200,2400,4800,9600,19200,38400,57600,115200


bit g_Frame_flag = 0;							// һ֡��ɱ�־
xdata uint8_t g_ReceiveBuf[MAX_L] = {0};		// ���ڽ����ݴ滺����
xdata uint16_t g_RxCnt = 0;						// ���ڽ������ݸ���
uint16_t g_Uart_time = 0;						// ���ڼ�ʱ

/************************************************
�������� �� UART_Timer1_Config
��    �� �� UART���ã��ö�ʱ��1�����Ϊ������ʱ��Դ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void UART_Timer1_Config(void)
{
	TMOD &= 0x0F;					// �����ʱ��1ģʽλ	
	TMOD |= 0x20;					// ģʽ2 --- 8λ�Զ���װ
	SM0 = 0;			
	SM1 = 1;						// ����Ϊ������ʽ 1
	REN = 1;						// ʹ�ܽ���
	
#ifdef TIMES_THE_SPEED
	PCON |= 0x80;					// �����ʼӱ�
	TL1 = -(FOSC/12/16/BAUDRATE);	// װ��ֵ
	
#else
	TL1 = -(FOSC/12/32/BAUDRATE);	// װ��ֵ
	
#endif /* TIMES_THE_SPEED */
	TH1 = TL1;	
	ET1 = 0;						// ��ֹ��ʱ��1�ж�
	IPH |= 0x10;
	PS = 1;							// ���ȼ� 3
	ES = 1;							// ʹ�ܴ����ж�
	TR1 = 1;						// ����ʱ�� 1
}

/************************************************
�������� �� UART_SendByte
��    �� �� �����ַ�����
��    �� �� c ---- ���͵�����
�� �� ֵ �� ��
*************************************************/
void UART_SendByte( uint8_t c )
{     
	SBUF = c;
    while(!TI);
	TI = 0;
}

/************************************************
�������� �� UART_SendString
��    �� �� �����ַ�������
��    �� �� pData ---- �ַ���
			Length ---- ����
�� �� ֵ �� ��
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
�������� �� UART_Printf
��    �� �� ���ڴ�ӡ���
��    �� �� String	---- �ַ���
�� �� ֵ �� ��
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
�������� �� UART_Receive
��    �� �� �����ַ�����
��    �� �� ��
�� �� ֵ �� Receive ---- ���յ�����
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
�������� �� UART_Isr
��    �� �� �����ж�
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void UART_ISR(void) interrupt 4
{
	uint8_t temp = 0;
	
	if(RI)
	{
		temp = UART_Receive();
		
		if(!g_Frame_flag)					// �ж�֡����������
		{
			g_ReceiveBuf[g_RxCnt] = temp;	
			g_RxCnt++;
			if(g_RxCnt >= MAX_L)
			{
				g_RxCnt = 0;				//Ϊ�˷�ֹ�������
			}
			g_Uart_time = 40;				//40ms(���ճ�ʱ�ж�)
		}				
	}
}


/*---------------------------- END OF FILE ----------------------------*/


