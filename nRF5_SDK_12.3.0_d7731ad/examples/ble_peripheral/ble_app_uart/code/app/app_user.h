#ifndef __APP_USER_H
#define __APP_USER_H

#include "global.h"

#define SECONDS2000YEAR				946656000	/* ʱ��(2000/01/01 00:00:00)�����Linuxʱ��(1970/01/01 08:00:00)��ʼ������ */

#ifdef FLASHUSERINFO
	#define MAX_USER_NUM			128			/* ����û����� */
#else
	#define MAX_USER_NUM			16			/* ����û����� */
#endif
#define MAX_LOG_NUM					100			/* �����־���� */

#ifdef PCB_L1_2_3
	#define HARD_VERSION_INFO		"HV1.2.3"	/* Ӳ���汾��Ϣ */
#elif defined PCB_UEL01020200
	#define HARD_VERSION_INFO		"HUEL01020200"	/* Ӳ���汾��Ϣ */
#elif defined PCB_L1_1_3
	#define HARD_VERSION_INFO		"HV1.1.3"	/* Ӳ���汾��Ϣ */
#else
	#define HARD_VERSION_INFO		"HV1.0.3"	/* Ӳ���汾��Ϣ */
#endif

#define SOFT_VERSION_INFO			"SV1.1.0-180806"	/* ����汾��Ϣ */

typedef struct 
{
	uint8_t  nowrunstate; // ʵʱ���е�״̬
	uint16_t advc; // ʵʱ��ѹֵ
	uint8_t  batterPercent;
	uint8_t  flagInit; // 0xA5-����ģʽ; 0x55-��λģʽ;
	uint8_t  flagAny; // ȫ�ֱ�־λ;Ӳ����ʼ��ʱ����;bit0-��NBģ���־λ;bit1-��SIM����־;
} SystemRunParam_t;

extern SystemRunParam_t gSystemRunParam;
extern uint16_t gFlagAdjustTime;


//==============================================================================
//������ SmartLock_User.c �еĺ���
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


