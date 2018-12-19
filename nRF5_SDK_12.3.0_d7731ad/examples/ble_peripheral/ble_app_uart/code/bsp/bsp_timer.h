#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "global.h"

#define MS_TO_S                 (100)
#define REWRITE_TIME            (180)
#define RE_OPEN_STA_PROC_TIME   (3600UL)

#define APP_RTC_TIMER_PRESCALER 0

typedef struct 
{	
    uint8_t  motor_on_timex1s;
    uint8_t  motor_off_timex1s;
    uint8_t  oled_timex1s;
	  uint16_t rtyCount;
    uint16_t rewriteCount;
    uint16_t reOpenStaProcCount;
}Tim_1s_Type;

typedef struct 
{
    uint8_t sys_timex100ms;
//    uint8_t clkWDIx100ms;                 //硬件看门狗用
}Tim_100ms_Type;

extern volatile Tim_1s_Type Tim_1s_Struct;
extern volatile Tim_100ms_Type Tim_100ms_Struct;


//系统慢速定时器
void start_systime(void);
void init_systime(void);
#endif
