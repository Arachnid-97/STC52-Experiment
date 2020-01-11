#ifndef __BSP_TIME_H
#define __BSP_TIME_H


#include "reg52.h"

#define COUNT_ENABLE            TR0 = 1
#define COUNT_DISABLE           TR0 = 0
#define TIM0_CNTR_CLEAR         TH0 = 0;TL0 = 0

#define PWM_GRADE				10U		// PWM×Ü¹² 10¼¶

extern uint32_t g_Time_1ms;
extern uint32_t g_Time_1s;
extern uint16_t g_Duty_Cycle;

void SoftwareDelay_ms( uint32_t Cnt );
void SoftwareDelay_5us( uint32_t Cnt );
void Delay_100us( uint32_t Cnt );
uint16_t TIM0_GetCounter(void);
void Time0_Config(void);
void Time1_Config(void);
void Time2_Config(void);


#endif /* __BSP_TIME_H */


/*------------------------------- END OF FILE -------------------------------*/


