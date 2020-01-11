#ifndef __CAT_TRACKING_H
#define __CAT_TRACKING_H


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

/* 寻迹 */
#define TRACE_LEFT			P3_6
#define TRACE_CENTRE		1 // P3_4		// 模块感应不好，不使用
#define TRACE_RIGHT			P3_7

extern bit g_Left_Adjust_flag;
extern bit g_Right_Adjust_flag;

void Left_Dispose(void);
void Right_Dispose(void);
void Infrared_Deal(void);


#endif /* __CAT_TRACKING_H */


/*---------------------------- END OF FILE ----------------------------*/


