#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include "global.h"

#define MS_TO_S                 (100)
#define REWRITE_TIME            (180)
#define RE_OPEN_STA_PROC_TIME   (3600UL)

#define APP_RTC_TIMER_PRESCALER 0

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
// 1st of January 2000 UTC
typedef uint32_t UTCTime;

/*********************************************************************
 * MACROS
 */

#define	IsLeapYear(yr)	(!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))

/*********************************************************************
 * MACROS
 */

#define	YearLength(yr)	(IsLeapYear(yr) ? 366 : 365)

/*********************************************************************
 * CONSTANTS
 */

// (MAXCALCTICKS * 5) + (max remainder) must be <= (uint16 max),
// so: (13105 * 5) + 7 <= 65535
#define MAXCALCTICKS  ((uint16)(13105))

#define	BEGYEAR	        2000     // UTC started at 00:00:00 January 1, 2000

#define	DAY             86400UL  // 24 hours * 60 minutes * 60 seconds

typedef struct 
{	
    uint8_t  motor_on_timex1s;
    uint8_t  motor_off_timex1s;
		uint8_t  motor_delay_count;
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

// To be used with
typedef struct
{
  uint8_t seconds;  // 0-59
  uint8_t minutes;  // 0-59
  uint8_t hour;     // 0-23
  uint8_t day;      // 0-30
  uint8_t month;    // 0-11
  uint16_t year;    // 2000+
} UTCTimeStruct;

extern volatile Tim_1s_Type Tim_1s_Struct;
extern volatile Tim_100ms_Type Tim_100ms_Struct;
extern uint8_t timestamp[8];

//系统慢速定时器
void start_systime(void);
void init_systime(void);
void osal_ConvertUTCTime( UTCTimeStruct *tm, UTCTime secTime );
#endif
