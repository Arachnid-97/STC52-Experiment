#ifndef __BSP_H
#define __BSP_H


#include "reg52.h"

/* °´¼ü */
#define KEY1				P1_4
#define KEY2				P1_5
#define KEY3				P1_6
#define KEY4				P1_7
#define KEY5				P3_5

/* LED */
#define LED1				P2_0
#define LED2				P2_1
#define LED3				P2_2
#define LED4				P2_3
#define LED5				P2_4
#define LED6				P2_5
#define LED7				P2_6
#define LED8				P2_7

/* ·äÃùÆ÷ */
#define BUZZER				P2_1

extern xdata uint8_t g_Key_Down;
extern uint8_t g_Key_time;

void Clk_Config(void);
void GPIO_Config(void);
void Bsp_Init(void);


#endif /* __BSP_H */


/*------------------------------- END OF FILE -------------------------------*/


