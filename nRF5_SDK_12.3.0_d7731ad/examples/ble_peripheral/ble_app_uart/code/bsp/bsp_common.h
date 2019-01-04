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
  uint8_t  writeRom_F        :1;    //如EEPROM写失败，则置起该标志
  uint8_t  writeDynamic_F    :1;
  uint8_t  atReday_F         :1;    //如GPRS 模块AT已准备OK,那么置起该标志
  uint8_t  closeStaProc      :1;    //如模块复位次数到达上限，那么暂时关闭GPRS状态处理，间隔一定时间再去执行
	uint8_t  motordelay        :1; 
  uint8_t  reserve           :3;
  
}Task_Flag_Type;


extern Task_Flag_Type Task_Flag_Struct;

uint8_t Com_Bsp_Init(void);

#endif

