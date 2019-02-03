#ifndef __BSP_SYSTEM_H
#define __BSP_SYSTEM_H

#include "global.h"

//#include "ble_self_servers.h"

//-----------各IO定义--------------------------
//#define SPI_SCK_PIN 		                   26
//#define SPI_MOSI_PIN 		                   25
//#define SPI_SS_PIN 			                   22

#define V_BAT_CTRL                         2
#define V_BAT_AD                           1
#define FB_5V                              4

#define LED1						                   8

#define SW3						                     10
#define SW2						                     11

#define INA					                       4
#define INB						                     5

#define MCU_UART3_RX                       6
#define MCU_UART3_TX						           7
#define NB_RESET    						           9
#define NB_POWER_ON                        3

#define TP1         						           16
#define TP2         						           15


////马达驱动用到的
//#define DRV_PPI_CHANNEL1      NRF_PPI_CHANNEL3   
//#define DRV_PPI_CHANNEL2      NRF_PPI_CHANNEL4
////-------------共用通道切换---------------------
//typedef enum 
//{ 
//    NOTHING_USE_CHANNEL,      
//    LOCKMOTOR_PWM_CHANNEL, 
//    LOCKMOTOR_FULL_CHANNEL, //上锁全速
//    UNLOCKMOTOR_FULL_CHANNEL, //开锁全速
//    UNLOCKMOTOR_PWM_CHANNEL,
//} use_channel_by_t;
//		
////----------全局标志位---------------------
//typedef union 
//{ 
//    uint32_t Whole;
//	struct
//    {
//        uint8_t work_in_lockst      :1; //上锁状态
//        uint8_t stop_adc_st         :1; //ADC采集完成
//        uint8_t vib_triggedst       :1;	//有震动触发
//        uint8_t start_adcsamplest   :1; //开启一次ADC采集
//        uint8_t MMA_is_activest     :1; //加速传感器激活态	
//        uint8_t stop_motor_st       :1;	//马达驱动结束
//        uint8_t reversed6           :1;		
//        uint8_t reversed7           :1;	
//            
//        uint8_t reversed8           :1;	
//        uint8_t reversed9           :1; 
//        uint8_t reversed10          :1;
//        uint8_t reversed11          :1;
//        uint8_t reversed12          :1;
//        uint8_t reversed13          :1;
//        uint8_t reversed14          :1;
//        uint8_t reversed15          :1;
//            
//        uint8_t reversed16          :1;
//        uint8_t exit_sleepst        :1; //触发退出低功耗 ，也就要开启定时
//        uint8_t timer_closest       :1; //驱动定时器已关闭
//        uint8_t reversed19          :1;
//        uint8_t reversed20          :1;
//        uint8_t reversed21          :1;
//        uint8_t reversed22          :1;
//        uint8_t reversed23          :1;

//        uint8_t reversed24          :1;
//        uint8_t fastadvertisingst   :1; //开始一次快速广播
//        uint8_t pressadvertisest    :1; //触发按键有按下
//        uint8_t sysble_connectingst :1; //蓝牙连接中
//        uint8_t cont_vibingst       :1; //连续震动测试中
//        uint8_t reversed29          :1;
//        uint8_t reversed30          :1;
//        uint8_t in_stop_advtisingst :1;	//无广播
//    }b;
//}app_mevent_t;

////主标志
//extern volatile app_mevent_t app_mevent;
////当前通道
//extern use_channel_by_t use_channel_by;

void init_peripheral(void);
void init_board(void);
//void exit_sleep_proc(void);

//void setWDI(void);


#endif
