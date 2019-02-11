#include "bsp_motor.h"
#include "nrf_drv_gpiote.h"

Motor_State_Tpye     Motor_State_Struct = STOP;
Motor_State_Tpye     Motor_OldStatus = STOP;
Motor_Tpye           Motor_Struct = {0};

void foreward_motor(void)
{
    nrf_gpio_pin_set(INA);	
    nrf_gpio_pin_clear(INB);	
}

void inversion_motor(void)
{
    nrf_gpio_pin_clear(INA);	
	  nrf_gpio_pin_set(INB);	
}

void stop_motor(void)
{
    nrf_gpio_pin_clear(INA);	
	  nrf_gpio_pin_clear(INB);	
}

static void Motor_Flicker(uint16_t usPeriod, uint16_t halfPeriod, uint8_t times)
{
//    uint8_t FlickerTimes = times*2;
    Motor_Struct.usCount +=usPeriod;
    if(Motor_Struct.usCount >= halfPeriod)
    {
        stop_motor();
				Motor_State_Struct = STOP;
        Motor_Struct.usCount = 0;     
//        if(times > 0)
//        {
//            LED_Struct.ucRollbackCnt++;
//            if(LED_Struct.ucRollbackCnt >= FlickerTimes)
//            {
//                LED_Struct.ucRollbackCnt = 0;
////                if(Charge_Flag.ChargeStatus == CHARGE_ENABLE)
////                {
////                    LED_Status = CHARGE_INDI;       /*充电LED指示*/
////                }
////                else
//                {
//                    LED_Status = SYS_INDI;          /*系统LED指示*/
//                }
//            }
//        }
     }
}

static void Motor_Indicate(uint16_t usPeriod)
{
    static uint8_t sucFlag=0; 
    switch(Motor_State_Struct)
    {
        case STOP:
        {
						stop_motor();   
        }
						break;
				
        case FOREWARD:
        {
						foreward_motor();
            Motor_Flicker(usPeriod, 2000, 0);   //第三项参数为0，表示闪烁次数不限
						Motor_OldStatus = FOREWARD;
        }
						break;
				
        case INVERSION:
        {
						inversion_motor();
						Motor_Flicker(usPeriod, 1800, 0);   //第三项参数为0，表示闪烁次数不限
						Motor_OldStatus = INVERSION; 
        }
						break;
				
        default:
						break;
     }

}

void Bsp_Motor_Tim(uint16_t usPeriod)
{
    Motor_Indicate(usPeriod);
}
