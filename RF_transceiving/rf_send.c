#include "rf_send.h"
#include "bsp_time.h"


/************************************************
�������� �� RF_Tx_Dispose
��    �� �� RF�źŷ���
��    �� �� RtnSig ---- ����
            RtnLen ---- ����
�� �� ֵ �� ��
*************************************************/
void RF_Tx_Dispose( const uint8_t *RtnSig, uint16_t RtnLen )
{
    uint8_t n,l;
    uint16_t i = 0;
	uint8_t temp;

	clr_EA;
    LEVEL_COMM = 1;
    Delay_100us(LOW_LEVEL_TIME);
    LEVEL_COMM = 0;
    Delay_100us(SYNC_CODE);												//ͬ���뷢��
    for(l = 0;l < RtnLen;l++)
    {
		temp = RtnSig[i];
        for(n = 0;n < 8;n++)							    			//bitת��ƽ
        {
            if(temp & 0x80)												// 1
            {
                LEVEL_COMM = 1; Delay_100us(HIGH_LEVEL_TIME);
                LEVEL_COMM = 0; Delay_100us(LOW_LEVEL_TIME);
            }
            else														// 0
            {
                LEVEL_COMM = 1; Delay_100us(LOW_LEVEL_TIME);
                LEVEL_COMM = 0; Delay_100us(HIGH_LEVEL_TIME);
            }
            temp <<= 1;
        }
        i++;
    }
    LEVEL_COMM = 0;
	set_EA;
}

/************************************************
�������� �� RF_Tx_Config
��    �� �� RF���ͳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void RF_Tx_Config(void)
{
	LEVEL_COMM = 0;
}


/*---------------------------- END OF FILE ----------------------------*/


