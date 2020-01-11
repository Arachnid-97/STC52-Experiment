#ifndef __RF_SEND_H
#define __RF_SEND_H


#include "reg52.h"

#define LEVEL_COMM          P3_2
#define SYNC_CODE			124
#define	HIGH_LEVEL_TIME     12
#define LOW_LEVEL_TIME		4


void RF_Tx_Config(void);
void RF_Tx_Dispose( const uint8_t *RtnSig, uint16_t RtnLen );

#endif /* __RF_SEND_H */


/*---------------------------- END OF FILE ----------------------------*/


