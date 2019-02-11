#ifndef  __BSP_MOTOR_H
#define  __BSP_MOTOR_H

#include "global.h"

typedef enum
{
		STOP = 0,
		FOREWARD,
		INVERSION,
}Motor_State_Tpye;

typedef struct
{
   uint16_t usCount;
   uint8_t  ucRollbackCnt;
	 uint16_t motor_times;
}Motor_Tpye;


extern Motor_State_Tpye      Motor_State_Struct;
extern Motor_State_Tpye      Motor_OldStatus;
extern Motor_Tpye            Motor_Struct;

void foreward_motor(void);
void inversion_motor(void);
void stop_motor(void);
void Bsp_Motor_Tim(uint16_t usPeriod);
#endif

