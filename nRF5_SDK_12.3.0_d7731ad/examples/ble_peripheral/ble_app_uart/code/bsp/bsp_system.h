#ifndef __BSP_SYSTEM_H
#define __BSP_SYSTEM_H

#include "global.h"

//#include "ble_self_servers.h"

//-----------��IO����--------------------------
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


////��������õ���
//#define DRV_PPI_CHANNEL1      NRF_PPI_CHANNEL3   
//#define DRV_PPI_CHANNEL2      NRF_PPI_CHANNEL4
////-------------����ͨ���л�---------------------
//typedef enum 
//{ 
//    NOTHING_USE_CHANNEL,      
//    LOCKMOTOR_PWM_CHANNEL, 
//    LOCKMOTOR_FULL_CHANNEL, //����ȫ��
//    UNLOCKMOTOR_FULL_CHANNEL, //����ȫ��
//    UNLOCKMOTOR_PWM_CHANNEL,
//} use_channel_by_t;
//		
////----------ȫ�ֱ�־λ---------------------
//typedef union 
//{ 
//    uint32_t Whole;
//	struct
//    {
//        uint8_t work_in_lockst      :1; //����״̬
//        uint8_t stop_adc_st         :1; //ADC�ɼ����
//        uint8_t vib_triggedst       :1;	//���𶯴���
//        uint8_t start_adcsamplest   :1; //����һ��ADC�ɼ�
//        uint8_t MMA_is_activest     :1; //���ٴ���������̬	
//        uint8_t stop_motor_st       :1;	//�����������
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
//        uint8_t exit_sleepst        :1; //�����˳��͹��� ��Ҳ��Ҫ������ʱ
//        uint8_t timer_closest       :1; //������ʱ���ѹر�
//        uint8_t reversed19          :1;
//        uint8_t reversed20          :1;
//        uint8_t reversed21          :1;
//        uint8_t reversed22          :1;
//        uint8_t reversed23          :1;

//        uint8_t reversed24          :1;
//        uint8_t fastadvertisingst   :1; //��ʼһ�ο��ٹ㲥
//        uint8_t pressadvertisest    :1; //���������а���
//        uint8_t sysble_connectingst :1; //����������
//        uint8_t cont_vibingst       :1; //�����𶯲�����
//        uint8_t reversed29          :1;
//        uint8_t reversed30          :1;
//        uint8_t in_stop_advtisingst :1;	//�޹㲥
//    }b;
//}app_mevent_t;

////����־
//extern volatile app_mevent_t app_mevent;
////��ǰͨ��
//extern use_channel_by_t use_channel_by;

void init_peripheral(void);
void init_board(void);
//void exit_sleep_proc(void);

//void setWDI(void);


#endif
