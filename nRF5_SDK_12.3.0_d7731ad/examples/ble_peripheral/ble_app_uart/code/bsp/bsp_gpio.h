#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "global.h"

///******  LED GPIO  DEFINE  OUTPUT ******/
//#define  LED_PORT                              GPIOC
//#define  LED_PIN                               GPIO_Pin_7
//#define  BSP_LED_LIGHT                         GPIO_ResetBits(LED_PORT ,LED_PIN);  
//#define  BSP_LED_DARK                          GPIO_SetBits(LED_PORT ,LED_PIN);
//#define  BSP_LED_FLICKER                       GPIO_ToggleBits(LED_PORT, LED_PIN);

///********VBUS CTL*******/
//#define  VBUS_CTL_PORT                         GPIOA
//#define  VBUS_CTL_PIN                          GPIO_Pin_6
////#define  BSP_VBUS_CTL_SET                      GPIO_SetBits(VBUS_CTL_PORT ,VBUS_CTL_PIN);                                           
////#define  BSP_VBUS_CTL_RESET                    GPIO_ResetBits(VBUS_CTL_PORT ,VBUS_CTL_PIN);
////#define  BSP_VBUS_CTL_READ                     GPIO_ReadInputDataBit(VBUS_CTL_PORT , VBUS_CTL_PIN)

//#define  CHARGE_OPEN()                          do\
//                                                {\
//                                                  GPIO_SetBits(VBUS_CTL_PORT,\
//                                                                VBUS_CTL_PIN);\
//                                                }while(0);

//#define  CHARGE_CLOSE()                         do\
//                                                {\
//                                                  GPIO_ResetBits(VBUS_CTL_PORT,\
//                                                                  VBUS_CTL_PIN );\
//                                                }while(0);

//                                                
///*****  GPRS GPIO ******/
#define  BSP_NB_RESET_SET                        nrf_gpio_pin_set(NB_RESET);                                          
#define  BSP_NB_RESET_CLEAR                      nrf_gpio_pin_clear(NB_RESET);

#define  BSP_NB_POWERON_SET                      nrf_gpio_pin_set(NB_POWER_ON);                                          
#define  BSP_NB_POWERON_CLEAR                    nrf_gpio_pin_clear(NB_POWER_ON);

//#define  GPRS_WAKE_UP_PORT                     GPIOG
//#define  GPRS_WAKE_UP_PIN                      GPIO_Pin_2

                                                
//void Bsp_GPIO_Init(void);

#endif










