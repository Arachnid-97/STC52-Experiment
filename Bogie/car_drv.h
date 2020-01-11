#ifndef __CAT_DRV_H
#define __CAT_DRV_H


#include "reg52.h"

#ifndef ENABLE
#define ENABLE              1
#endif /* ENABLE */

#ifndef DISABLE
#define DISABLE             0
#endif /* DISABLE */

#ifndef HIGH
#define HIGH                1
#endif /* HIGH */

#ifndef LOW
#define LOW                 0
#endif /* LOW */

/* µç»úPORT */
#define MOTOR_OUT1(x)		(P1_4 = x)
#define MOTOR_OUT2(x)		(P1_5 = x)
#define MOTOR_OUT3(x)		(P1_6 = x)
#define MOTOR_OUT4(x)		(P1_7 = x)

#define PWM_CLASS			6

extern bit g_Run_flag;
extern bit g_Stop_flag;
extern bit g_Left_flag;		
extern bit g_Right_flag;
extern uint8_t g_Function;

void Run(void);
void BackRun(void);
void LeftRun(void);
void RightRun(void);
void Stop(void);
void Car_Control(void);
void Car_Function( uint16_t Mode );
void Car_Init(void);


#endif /* __CAT_DRV_H */


/*---------------------------- END OF FILE ----------------------------*/


