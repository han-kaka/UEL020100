#ifndef __BSP_BATTERY_ADC_H
#define __BSP_BATTERY_ADC_H

#include "global.h"

//#define ADC_SAMPLING_TIME_100MS     10
//#define ALARM_TEMPERATURE_VALUE     5555
#define ALARM_VOLTAGE_VALUE         370   

//#define CHARGE_BATTERY_VALUE (uint32_t)0x90//0x08a
#define MAX_BATTERY_VALUE  (uint32_t)0x0400  //1.2V 845
#define MIN_BATTERY_VALUE  (uint32_t)0x2a0//0x0289  //3.3V
//#define NORMAL_BATTERY_VALUE (uint32_t)0x02fa //3.7V

//#define LOW_BATTERY_VALUE  (uint32_t)0x02c0 //3.4V

//#define CLOSE_GPRS_VALUE  (uint32_t)0x02C3 //3.45V 706
//#define RESET_GPRS_VALUE  (uint32_t)0x02E0 //3.6V


//typedef struct
//{
////		uint16_t voltage_solar; 
//		uint16_t voltage_battery;
////		int16_t  temp_value;
//}AD_Type;

typedef enum
{
    ADC_START       = 0x01,
    ADC_SAMPLING    = 0x02,
    ADC_STOP        = 0x03
}ADC_Sample_Enum;

//typedef struct
//{ 
//    uint16_t voltage_ref;
//    uint16_t voltage_ntc;
////    uint16_t voltage_battery;
////    uint16_t voltage_solar; 
//}voltage_adc_t;

typedef enum 
{    
		RT_REFER_ADC,      
		NTC_ADC, 
		SOLAR_ADC, 
		BATTERY_ADC,
		IDLE_ADC,
}Current_ADC_Enum;

void measure_voltage(void);

#endif

