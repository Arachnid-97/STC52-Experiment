#ifndef __CAT_AVOID_H
#define __CAT_AVOID_H


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

/* 红外避障 */
#define INFRARED_LEFT		P2_4
#define INFRARED_RIGHT		P2_3
#define INFRARED_QUEEN		P2_2

/* 舵机 */
#define SERVO(x)			(P2_0 = x)	// 舵机控制
#define CENTER				11			// 90度
#define LEFT_CORNER			25			// 0度
#define RIGHT_CORNER		4			// 180度
#define TOP_LEFT_CORNER		20			// 45度
#define TOP_RIGHT_CORNER	10			// 135度

#define DISTANCE_MAX		500
#define DISTANCE_Min		25

extern uint8_t g_Servo_Angle;
extern uint16_t g_Servo_Time;

void Servo_Control( uint8_t Time );
bit Servo_Deal(void);
bit Obstacle_Deal(void);


#endif /* __CAT_AVOID_H */


/*---------------------------- END OF FILE ----------------------------*/


