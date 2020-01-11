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

/* ������� */
#define INFRARED_LEFT		P2_4
#define INFRARED_RIGHT		P2_3
#define INFRARED_QUEEN		P2_2

/* ��� */
#define SERVO(x)			(P2_0 = x)	// �������
#define CENTER				11			// 90��
#define LEFT_CORNER			25			// 0��
#define RIGHT_CORNER		4			// 180��
#define TOP_LEFT_CORNER		20			// 45��
#define TOP_RIGHT_CORNER	10			// 135��

#define DISTANCE_MAX		500
#define DISTANCE_Min		25

extern uint8_t g_Servo_Angle;
extern uint16_t g_Servo_Time;

void Servo_Control( uint8_t Time );
bit Servo_Deal(void);
bit Obstacle_Deal(void);


#endif /* __CAT_AVOID_H */


/*---------------------------- END OF FILE ----------------------------*/


