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

int8_t   UserMemCmp( const void *src1, const void *src2, unsigned int len );

#endif


