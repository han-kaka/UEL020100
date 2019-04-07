#ifndef __BSP_SYSTEM_H
#define __BSP_SYSTEM_H

#include "global.h"

//-----------¸÷IO¶¨Òå--------------------------
#define V_BAT_CTRL                         2
#define V_BAT_AD                           1
#define FB_5V                              4

#define LED1						                   8

#define SW1						                     12
#define SW2						                     13

#define INA					                       4
#define INB						                     5

#define MCU_UART3_RX                       6
#define MCU_UART3_TX						           7
#define NB_RESET    						           9
#define NB_POWER_ON                        3

#define TP1         						           16
#define TP2         						           15

void init_peripheral(void);
void init_board(void);

#endif
