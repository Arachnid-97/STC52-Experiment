#include "rf_receive.h"
#include "bsp.h"
#include "bsp_time.h"
#include "bsp_eeprom.h"
#include "bsp_uart.h"

uint8_t g_RF_Study_flag = 0;                          // RF学习标志
uint8_t g_RF_Study_Time = 0;                          // RF学习状态时间计时
uint8_t g_RF_Wait_Time = 0;                           // RF等待时间
uint8_t g_RF_Control = 1;                             // 遥控对应控制 ---- 预留
uint8_t g_RF_KeyData = 0;                             // 数据码
uint8_t g_RF_DataBuff[2][3] = {0};                    // RF数据缓冲区(处理完后 0行为地址数据， 1行为完整数据)

static uint8_t s_RF_DecodeOK_flag = 0;               // 解码完成标志
static uint8_t s_RF_Type = 0x29;                     // 编码类型 1257 -- 0x08/ 2262 -- 0x29
static uint8_t s_RF_KeyBuff[64] = {0};               // 已学习的按键码

/************************************************
函数名称 ： RF_Decode
功    能 ： RF数据解码(兼容 2262/ 1527)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Decode(void)
{

#if 0
    uint8_t i;
    uint8_t row = 0;
    uint8_t column = 0;
    uint32_t low_width = 0;
    uint32_t high_width = 0;

    g_RF_KeyData = 0;
    TIM2_CNTR_CLEAR;                               // 计数清零

    COUNT_ENABLE;                                  // 启动计时器
    while(RF_SIGNAL)                               // 测基准电平
    {
        if(TIM2_CNTRH >= 0x0F)
        {
            COUNT_DISABLE;
            TIM2_CNTR_CLEAR;
            return ;
        }
    }
//    COUNT_DISABLE;
    high_width = TIM2_CNTRH;
    high_width = high_width*256 + TIM2_CNTRL;
    TIM2_CNTR_CLEAR;

//    COUNT_ENABLE;
    while(!RF_SIGNAL)                              // 测同步码宽度
    {
        if(TIM2_CNTRH >= 0x76)
        {
            COUNT_DISABLE;
            TIM2_CNTR_CLEAR;
            return ;
        }
    }
//    COUNT_DISABLE;
    low_width = TIM2_CNTRH;
    low_width = low_width*256 + TIM2_CNTRL;

//    high_width = low_width / 31;                  // 基准宽度

    if((low_width > 8000) && (low_width < 11000))
//    if(((high_width*28) < low_width) && (low_width < (high_width*34)))   //同步码（引导码宽度是窄脉冲的31倍）
    {
        asm("sim\n");

        for(row = 0;row < 2;row++)
        {
            for(column = 0;column < 3;column++)
            {
                for(i = 0;i < 8;i++)
                {
                    low_width = 0;
                    TIM2_CNTR_CLEAR;
//                    COUNT_ENABLE;
                    while(RF_SIGNAL)                // 高电平等待
                    {
                        if(TIM2_CNTRH >= 0x0F)
                        {
                            COUNT_DISABLE;
                            TIM2_CNTR_CLEAR;
                            asm("rim\n");
                            return ;
                        }
                    }
//                    COUNT_DISABLE;
                    TIM2_CNTR_CLEAR;

//                    COUNT_ENABLE;
                    while(!RF_SIGNAL)               // 测低电平宽度
                    {
                        if(TIM2_CNTRH >= 0x0F)
                        {
                            COUNT_DISABLE;
                            TIM2_CNTR_CLEAR;
                            asm("rim\n");
                            return ;
                        }
                    }
//                    COUNT_DISABLE;
                    low_width = TIM2_CNTRH;
                    low_width = low_width*256 + TIM2_CNTRL;
                    TIM2_CNTR_CLEAR;

                    if((low_width > 100) && (500 > low_width))
//                    if((low_width > high_width*0.9) && (high_width*1.25 > low_width))
                    {
                        g_RF_DataBuff[row][column] |= (1 << (7 - i));           //置1
                    }
                    else if((600 < low_width) && (low_width < 1200))
//                    else if((3*high_width*0.9 < low_width) && (low_width < 3*high_width*1.25))
                    {
                        g_RF_DataBuff[row][column] &= ~(1 << (7 - i));          //置0
                    }
                    else
                    {
                        COUNT_DISABLE;
                        TIM2_CNTR_CLEAR;
                        asm("rim\n");
                        return ;                    // 乱码退出
                    }
                }
            }
            low_width = 0;
//            COUNT_ENABLE;
            TIM2_CNTR_CLEAR;
            while(RF_SIGNAL)
            {
                if(TIM2_CNTRH >= 0x0F)
                {
                    COUNT_DISABLE;
                    TIM2_CNTR_CLEAR;
                    asm("rim\n");
                    return ;
                }
            }
//            COUNT_DISABLE;
            TIM2_CNTR_CLEAR;

//            COUNT_ENABLE;
            while(!RF_SIGNAL)
            {
                if(TIM2_CNTRH >= 0x76)
                {
                    COUNT_DISABLE;
                    TIM2_CNTR_CLEAR;
                    asm("rim\n");
                    return ;
                }
            }
//            COUNT_DISABLE;
            low_width = TIM2_ARRH;
            low_width = low_width*256 + TIM2_ARRL;
            TIM2_CNTR_CLEAR;

            if((low_width > 8000) && (low_width < 11000))
//            if((low_width < (high_width*28)) || ((high_width*34) < low_width))
            {
                COUNT_DISABLE;
                TIM2_CNTR_CLEAR;
                asm("rim\n");
                return ;                            // 同步码错误退出
            }
        }
        if((g_RF_DataBuff[0][0] == g_RF_DataBuff[1][0]) &&
               (g_RF_DataBuff[0][1] == g_RF_DataBuff[1][1]) &&
                   (g_RF_DataBuff[0][2] == g_RF_DataBuff[1][2]))    //数据比较
        {
            LED = ~LED;
            s_RF_DecodeOK_flag = 1;
        }
        asm("rim\n");
    }
    COUNT_DISABLE;
    TIM2_CNTR_CLEAR;

#else
    static uint8_t row = 0;
    static uint8_t column = 0;
    static uint8_t data_bit=0;
    static uint8_t elec_level_last = 0;
    static uint16_t head = 0;
    static uint16_t low_width = 0;
    static uint16_t high_width = 0;
    static uint32_t period = 0;
    static uint32_t RF_time = 0;
//    uint Ratio=0;

    RF_time = TIM2_CNTRH;
    RF_time = (RF_time << 8) + TIM2_CNTRL;
    TIM2_CNTR_CLEAR;
    COUNT_ENABLE;

    if(RF_SIGNAL)
    {
        if(elec_level_last)                 // 电平有变化
        {
            low_width = RF_time;

            if(!head)
            {
                if(((low_width / high_width) > 26) && ( 36 < (low_width / high_width)))
//                if(((high_width*26) < low_width) && (low_width < (high_width*36)))   // 同步码（引导码宽度是窄脉冲的31倍）
//                if((RF_time >= 8000)&&(RF_time <= 11000))
                {
                    data_bit = 0;
                    head = RF_time;
                }
            }
            else
            {
                column = (data_bit / 8);
                g_RF_DataBuff[row][column] = g_RF_DataBuff[row][column] << 1;        // 数据左移一位

                period = high_width + low_width;
                if((period > head / 9) && (head / 7 > period))
                {
                    if(high_width > low_width)
//                    if((low_width > 150) && (low_width < 600))                           // 窄脉冲判断
                    {
                        g_RF_DataBuff[row][column] = g_RF_DataBuff[row][column] | 0x01;     // 置一
                    }
                    else
//                    else if((low_width > 650) && (low_width < 1200))                    // 宽脉冲判断
                    {
                        g_RF_DataBuff[row][column] = g_RF_DataBuff[row][column] & 0xfe;     // 置零
                    }
                }
                else                                                                // 接收错误
                {
                    data_bit = 0;
                    head = 0;
                    column = 0;
                    row = 0;
                    COUNT_DISABLE;
                    return ;
                }
                data_bit++;                                                     //字节位加一
                if(data_bit == 24)
                {
                    data_bit = 0;
                    head = 0;
                    column = 0;

                    if((g_RF_DataBuff[0][0] == g_RF_DataBuff[1][0]) &&
                           (g_RF_DataBuff[0][1] == g_RF_DataBuff[1][1]) &&
                               (g_RF_DataBuff[0][2] == g_RF_DataBuff[1][2]))    //数据比较
                    {
                        s_RF_DecodeOK_flag = 1;
                    }

                    if(row > 0)
                    {
                        row = 0;
                    }
                    else
                    {
                        row++;
                    }
                    COUNT_DISABLE;

                }

            }
        }
        elec_level_last = 0;
    }
    else
    {
        if(!elec_level_last)
        {
            if((RF_time > 40) && (RF_time < 1270))
            {
                high_width = RF_time;
            }
        }
        elec_level_last = 1;
    }

#endif
}

/************************************************
函数名称 ： RF_Rx_Dispose
功    能 ： RF接收状态处理
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Rx_Dispose(void)
{
    uint8_t n,j,k,q;
    uint8_t RF_key_num = 0;

    if(g_RF_Study_flag)                             // 学习状态
    {
        if(g_RF_Study_Time > 20)
        {
            g_RF_Study_Time = 0;
            g_RF_Study_flag = 0;
            return ;                               // 10 秒超时退出
        }
        else if(s_RF_DecodeOK_flag)
        {
            g_RF_Study_flag = 0;
            s_RF_DecodeOK_flag = 0;
            RF_key_num = s_RF_KeyBuff[0];          // 取已学习的遥控器数量
            if(RF_key_num > 12)
            {
                RF_key_num = 0;                    // 如果遥控器数量超过12个，覆盖最先学习的
            }
            s_RF_KeyBuff[RF_key_num*5 + 1] = s_RF_Type;
            s_RF_KeyBuff[RF_key_num*5 + 2] = g_RF_DataBuff[1][0];
            s_RF_KeyBuff[RF_key_num*5 + 3] = g_RF_DataBuff[1][1];
            s_RF_KeyBuff[RF_key_num*5 + 4] = g_RF_DataBuff[1][2];
            s_RF_KeyBuff[RF_key_num*5 + 5] = g_RF_Control;
            EEPROM_WriteNByte(&s_RF_KeyBuff[RF_key_num*5 + 1],(RF_key_num*5 + 1), 5);    //更新数据到EEPROM

            RF_key_num ++;
            s_RF_KeyBuff[0] = RF_key_num;
            EEPROM_WriteNByte(&RF_key_num, 0, 1);  // 更新数量
            LED_ON;
        }
    }
    else
    {
        if(s_RF_DecodeOK_flag)
        {
            s_RF_DecodeOK_flag = 0;

            if(g_RF_Wait_Time > 200)					// 连按判断
            {
                for(j = 0;j < 3;j++)                    // 判断 2262与 1527
                {
                    for(k = 0;k < 4;k++)
                    {
                        if(((g_RF_DataBuff[0][j] >> (k*2)) & 3) == 2)
                        {
                            j = 152;                    // 有 10数据则为1527（10在 2262为无效码）
                            break;
                        }
                    }
                    if(j == 152)
                    {
                        break;
                    }
                }
                if(j == 152)                            // ev1527
                {
                    g_RF_KeyData = g_RF_DataBuff[1][2] & 0x0F;              //提取码键
                    g_RF_DataBuff[0][2] = g_RF_DataBuff[1][2] >> 4;         //提取地址码
                    s_RF_Type = 0x08;
                }
                else                                    // pt2262
                {
                    for(q = 0;q < 4;q++)
                    {
                        if(((g_RF_DataBuff[1][2] >> (q*2)) & 3) == 3)      //（11为 1，00为 0）
                        {
                            g_RF_KeyData |= 1 << q;     // 写 1
                        }
                    }
                    g_RF_DataBuff[0][2] = 0x00;         // 2262无后 4位地址,全为 0
                    s_RF_Type = 0x29;
                }

                for(n = 0;n < 12;n++)                   // 查找已学习按键
                {
                    if((g_RF_DataBuff[1][0] == s_RF_KeyBuff[n*5 + 2]) &&
                           (g_RF_DataBuff[1][1] == s_RF_KeyBuff[n*5 + 3]) &&
                               (g_RF_DataBuff[1][2] == s_RF_KeyBuff[n*5 + 4]))
                    {
                        LED_ON;
                    }
                }
                LED = ~LED;
            }
            g_RF_Wait_Time = 0;
        }
    }
}

/************************************************
函数名称 ： RF_Rx_Config
功    能 ： RF接收 IO配置(PD4)
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Rx_Config(void)
{
    PD_DDR_DDR4 = 0;                 // 输入模式
    PD_CR1_C14 = 1;                  // 上拉输入
    PD_CR2_C24 = 1;                  // 使能中断
    EXTI_CR1_PDIS = 3;               // 下沿触发
    ITC_SPR2_VECT6SPR = 0;           // 级别 2
}

/************************************************
函数名称 ： RF_Rx_Init
功    能 ： RF接收初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void RF_Rx_Init(void)
{
    RF_Rx_Config();
//    EEPROM_ReadNByte(s_RF_KeyBuff, 0, 64);
}


/*---------------------------- END OF FILE ----------------------------*/


