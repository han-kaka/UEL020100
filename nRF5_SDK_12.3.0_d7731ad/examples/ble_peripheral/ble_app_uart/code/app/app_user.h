#ifndef __APP_USER_H
#define __APP_USER_H

#include "global.h"

#define SECONDS2000YEAR				946656000	/* 时间(2000/01/01 00:00:00)相对于Linux时间(1970/01/01 08:00:00)开始的秒数 */

#ifdef FLASHUSERINFO
	#define MAX_USER_NUM			128			/* 最大用户数量 */
#else
	#define MAX_USER_NUM			16			/* 最大用户数量 */
#endif
#define MAX_LOG_NUM					100			/* 最大日志数量 */

#ifdef PCB_L1_2_3
	#define HARD_VERSION_INFO		"HV1.2.3"	/* 硬件版本信息 */
#elif defined PCB_UEL01020200
	#define HARD_VERSION_INFO		"HUEL01020200"	/* 硬件版本信息 */
#elif defined PCB_L1_1_3
	#define HARD_VERSION_INFO		"HV1.1.3"	/* 硬件版本信息 */
#else
	#define HARD_VERSION_INFO		"HV1.0.3"	/* 硬件版本信息 */
#endif

#define SOFT_VERSION_INFO			"SV1.1.0-180806"	/* 软件版本信息 */

typedef struct 
{
	uint8_t  nowrunstate; // 实时运行的状态
	uint16_t advc; // 实时电压值
	uint8_t  batterPercent;
	uint8_t  flagInit; // 0xA5-出厂模式; 0x55-复位模式;
	uint8_t  flagAny; // 全局标志位;硬件初始化时清零;bit0-有NB模块标志位;bit1-有SIM卡标志;
} SystemRunParam_t;

extern SystemRunParam_t gSystemRunParam;
extern uint16_t gFlagAdjustTime;

int8_t   UserMemCmp( const void *src1, const void *src2, unsigned int len );

#endif


