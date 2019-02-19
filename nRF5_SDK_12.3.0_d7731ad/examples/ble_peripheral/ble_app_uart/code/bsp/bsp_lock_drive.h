#ifndef __BSP_LOCK_DRIVE_H
#define __BSP_LOCK_DRIVE_H
#include "bsp.h"

#include "global.h"

////------x64us
//#define UNLOCK_FULL_DRIVE_TIME  6630// //最长0.425秒全速开锁
//#define UNLOCK_PWM_DRIVE_TIME  12000 //最长2.5秒半速开锁
//#define LOCK_FULL_DRIVE_TIME  6630//0.42秒全速上锁 
//#define LOCK_PWM_DRIVE_TIME   7812//0.5S半速上锁

//#define TESTUNLOCKKEY_TIME     80//5毫秒
//#define TESTMIDDLEKEY_TIME     60//3.6  
//#define LOCK_MOTOR_PWM_WITDTH_LOW    50 //78%
//#define UNLOCK_MOTOR_PWM_WITDTH_LOW    55 //86%

typedef struct 
{
		uint8_t                            work_in_lockst :1;

}Motor_State_Type;

//-------------共用通道切换---------------------
typedef enum 
{ 
    NOTHING_USE_CHANNEL,      
    LOCKMOTOR_PWM_CHANNEL, 
    LOCKMOTOR_FULL_CHANNEL, //上锁全速
    UNLOCKMOTOR_FULL_CHANNEL, //开锁全速
    UNLOCKMOTOR_PWM_CHANNEL,
}Use_Channel_Type;

extern Motor_State_Type motor_state_struct;
extern Use_Channel_Type use_channel_enum;

//void turnToFreeChannel(void);
//void release_drive_channel(void);
void setup_drive_channel(void);

void bsp_open_motor_full_setup(void);
void bsp_close_motor_full_setup(void);
//void bsp_unlockmotor_pwm_setup(void);
//void bsp_lockmotor_pwm_setup(void);

//void manual_testworkst(void);
//bool testDriveOpenKey(void);

//void stop_lockmotor_pwm(void);
//void stop_unlockmotor_pwm(void);

//bool open_motor_task(void);
//bool open_fencing_task(void);


void drive_open(void);
//void drive_closelock(void);	
//void drive_halfCloseLock(void);
void drive_half_open(void);
#endif
