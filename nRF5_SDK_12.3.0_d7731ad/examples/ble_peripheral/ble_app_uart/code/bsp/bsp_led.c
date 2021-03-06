#include "bsp_led.h"
#include "nrf_drv_gpiote.h"

LED_Status_Tpye     LED_Status = SYS_INDI;
LED_Status_Tpye     LED_OldStatus = SYS_INDI;
LED_Tpye            LED_Struct={0};

void bsp_led_init(void)
{
    nrf_gpio_cfg_output(LED1);
}

void bsp_led_1_off(void)
{
    nrf_gpio_pin_clear(LED1);	
}

void bsp_led_1_on(void)
{
    nrf_gpio_pin_set(LED1);
}

void bsp_led_1_toggle(void)
{
    nrf_gpio_pin_toggle(LED1);
}

static void Led_Indicate(uint16_t usPeriod)
{
    static uint8_t sucFlag=0; 
    switch(LED_Status)
    {
//        case SYS_INDI:
//        {
//             BSP_LED_DARK;   
//        }
//						break;
//				
//        case CONN_INDI:
//        {
//             sucFlag = ENABLE;
//             Led_Flicker(usPeriod,1000,0);   //第三项参数为0，表示闪烁次数不限
//        }
//						break;
//				
//        case DATA_SEND_INDI:
//        {
//             /*当网络连接状态过渡到通讯状态时，LED_Struct.usCount变量可能不为0*/
//             if(sucFlag != DISABLE)        
//             {
//                LED_Struct.usCount = 0;
//                sucFlag = DISABLE;
//             }
//             Led_Flicker(usPeriod,200,4);    
//        }
//						break;
//				
//        case CHARGE_INDI:
//        {
//             BSP_LED_LIGHT;   
//        }
//						break;
//				
        default:
						break;
     }

}

void Bsp_Led_Tim(uint16_t usPeriod)
{
    Led_Indicate(usPeriod);
}


