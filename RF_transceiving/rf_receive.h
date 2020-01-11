#ifndef __RF_RECEIVE_H
#define __RF_RECEIVE_H


#include "reg52.h"

#define RF_SIGNAL               PD_IDR_IDR4

extern uint8_t g_RF_Study_flag;
extern uint8_t g_RF_Study_Time;
extern uint8_t g_RF_Wait_Time;s
extern uint8_t g_RF_Control;
extern uint8_t g_RF_KeyData;
extern uint8_t g_RF_DataBuff[2][3];

void RF_Decode(void);
void RF_Rx_Dispose(void);
void RF_Rx_Config(void);
void RF_Rx_Init(void);


#endif /* __RF_RECEIVE_H */


/*---------------------------- END OF FILE ----------------------------*/


