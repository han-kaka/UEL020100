#ifndef  __BSP_MOTOR_H
#define  __BSP_MOTOR_H

#include "global.h"

typedef enum
{
		STOP = 0,
		FOREWARD,
		INVERSION,
}Motor_Status_Tpye;

typedef struct
{
   uint16_t usCount;
   uint8_t  ucRollbackCnt;
}Motor_Tpye;


extern Motor_Status_Tpye      Motor_Status;

void Foreward_Motor(void);
void Inversion_Motor(void);
void Stop_Motor(void);
void Bsp_Motor_Tim(uint16_t usPeriod);
#endif

