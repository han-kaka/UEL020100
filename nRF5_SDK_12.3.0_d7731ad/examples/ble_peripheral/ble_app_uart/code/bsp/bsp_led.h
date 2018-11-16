#ifndef  __BSP_LED_H
#define  __BSP_LED_H

#include "global.h"

typedef enum
{
  SYS_INDI = 0,
  CONN_INDI,
  DATA_SEND_INDI,
  CHARGE_INDI,
  MAX_INDI,
}LED_Status_Tpye;

typedef struct
{
   uint16_t usCount;
   uint8_t  ucRollbackCnt;
}LED_Tpye;


extern LED_Status_Tpye      LED_Status;

void bsp_led_init(void);

void bsp_led_1_on(void);
void bsp_led_1_off(void);
void bsp_led_1_toggle(void);

void Bsp_Led_Tim(uint16_t usPeriod);

#endif
