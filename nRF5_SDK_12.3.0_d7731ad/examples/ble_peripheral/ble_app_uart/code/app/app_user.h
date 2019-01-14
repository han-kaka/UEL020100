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


//==============================================================================
//定义在 SmartLock_User.c 中的函数
//==============================================================================
//uint32 SC_GenStrToInt(uint8 *pdat);
//uint16 SC_Strlen(uint8 *ch);
//uint16 SC_StrPrintlen(uint8 *ch);
//uint8* SC_Strstr(uint8 *ch1, uint8 *ch2, uint16 len1, uint16 len2);
int8_t   UserMemCmp( const void *src1, const void *src2, unsigned int len );
//uint8  UserSearchUserInfoNumber(uint8 *id);
//uint8  UserSearchUserInfoUID(uint16 num, uint8 *id);
uint8_t UserReadUserInfoUID(uint8_t num, uint8_t *uid);
uint8_t UserReadUserInfoConfig(uint8_t *uid, uint8_t *dat);
//uint8  UserAddUserInfoToSystem(uint8 type, uint8 *id, uint8 *pData);
//uint8  UserDelUserInfoFromSystem(uint8 *uid);
//void   UserClearUserInfoFromSystem(void);
void   UserSetCurrentUser(uint8_t *uid);
uint8_t* UserGetCurrentUser(void);
void   UserClrCurrentUser(void);
//void   FeetDog(void);
//void   UserHalInit(void);
//void   UserSoftInit(void);
//void   Save_SysRunState(uint8 sta);
//uint8  Get_SysRunState(void);
//void   WaitSystemExit(uint8 type, uint32 ms);
//void   NowSystemExit(void);
//void   UserHardDelayNop(uint16 us);
//void   UserHardDelayMs(uint16 ms);
//void   UserHardWDGDelayMs(uint16 ms);
//void   TestBug(uint8 bugcont);

#endif


