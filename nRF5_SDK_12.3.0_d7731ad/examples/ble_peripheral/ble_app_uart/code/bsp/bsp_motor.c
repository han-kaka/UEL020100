#include "bsp_motor.h"
#include "nrf_drv_gpiote.h"

Motor_Status_Tpye     Motor_Status = STOP;
Motor_Status_Tpye     Motor_OldStatus = STOP;
Motor_Tpye            Motor_Struct={0};

void Foreward_Motor(void)
{
    nrf_gpio_pin_set(INA);	
    nrf_gpio_pin_clear(INB);	
}

void Inversion_Motor(void)
{
    nrf_gpio_pin_clear(INA);	
	  nrf_gpio_pin_set(INB);	
}

void Stop_Motor(void)
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
        Stop_Motor();
				Motor_Status = STOP;
        Motor_Struct.usCount = 0;        
//        if(times > 0)
//        {
//            LED_Struct.ucRollbackCnt++;
//            if(LED_Struct.ucRollbackCnt >= FlickerTimes)
//            {
//                LED_Struct.ucRollbackCnt = 0;
////                if(Charge_Flag.ChargeStatus == CHARGE_ENABLE)
////                {
////                    LED_Status = CHARGE_INDI;       /*���LEDָʾ*/
////                }
////                else
//                {
//                    LED_Status = SYS_INDI;          /*ϵͳLEDָʾ*/
//                }
//            }
//        }
     }
}

static void Motor_Indicate(uint16_t usPeriod)
{
    static uint8_t sucFlag=0; 
    switch(Motor_Status)
    {
        case STOP:
        {
						Stop_Motor();   
        }
						break;
				
        case FOREWARD:
        {
						Foreward_Motor();
            Motor_Flicker(usPeriod, 1000, 0);   //���������Ϊ0����ʾ��˸��������
        }
						break;
				
        case INVERSION:
        {
						Inversion_Motor();
						Motor_Flicker(usPeriod, 1000, 0);   //���������Ϊ0����ʾ��˸��������
//             //����������״̬���ɵ�ͨѶ״̬ʱ��LED_Struct.usCount�������ܲ�Ϊ0
//             if(sucFlag != DISABLE)        
//             {
//                LED_Struct.usCount = 0;
//                sucFlag = DISABLE;
//             }
//             Led_Flicker(usPeriod, 200, 4);    
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
