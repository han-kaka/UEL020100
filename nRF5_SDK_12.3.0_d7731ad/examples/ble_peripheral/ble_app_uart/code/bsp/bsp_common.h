#ifndef __BSP_COMMON_H
#define __BSP_COMMON_H

#include "global.h"

//#include "bsp_system.h"
//#include "bsp_timer.h"
//#include "bsp_ble_trans.h"
//#include "bsp_led.h"
//#include "bsp_var.h"
//#include "bsp_mem.h"
//#include "bsp_gpio.h"
//#include "bsp_my_uart.h"
//#include "bsp_motor.h"
//#include "bsp_flash_rw.h"
//#if SEGGER_RTT_DEBUG_BLUETOOTH
//#include "bsp_rtt_debug.h"
//#endif


//#include "app_common.h"
//#include "task_common.h"


typedef struct
{
  uint8_t  writeRom_F        :1;    //��EEPROMдʧ�ܣ�������ñ�־
  uint8_t  writeDynamic_F    :1;
  uint8_t  atReday_F         :1;    //��GPRS ģ��AT��׼��OK,��ô����ñ�־
  uint8_t  closeStaProc      :1;    //��ģ�鸴λ�����������ޣ���ô��ʱ�ر�GPRS״̬�������һ��ʱ����ȥִ��
	uint8_t  motordelay        :1; 
  uint8_t  reserve           :3;
  
}Task_Flag_Type;


extern Task_Flag_Type Task_Flag_Struct;

uint8_t Com_Bsp_Init(void);

#endif

