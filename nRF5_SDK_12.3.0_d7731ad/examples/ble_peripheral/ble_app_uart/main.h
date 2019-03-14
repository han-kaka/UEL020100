/******************************************************************
** 版权:       
** 文件名:     main.h
** 版本：
** 工作环境:   KEIL_5.23
** 作者:       HAN
** 生成日期:   2018.8.15
** 功能:       文件包含
** 相关文件:
** 修改日志:
*******************************************************************/

#ifndef  __MAIN_H
#define  __MAIN_H

#define AES_KEY_LEN                         (16)

#define IMEI_LEN                            (16)
#define ICCID_LEN                           (32)
#define MAC_LEN                             (6)

#define SERVER_APN_LEN                      (16)
#define SERVER_IP_LEN                       (32)
#define LOG_INDEX_LEN                       (5)

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <math.h>
//#include <time.h>

#include "SEGGER_RTT.h"
#include "nrf_log.h"

#if SEGGER_RTT_DEBUG_BLUETOOTH
#include "bsp_rtt_debug.h"
#endif
#include "bsp_flash_rw.h"
#include "bsp_my_uart.h"
#include "bsp_motor.h"
#include "bsp_gpio.h"
#include "bsp_mem.h"
#include "bsp_var.h"
#include "bsp_led.h"
#include "bsp_ble_trans.h"
#include "bsp_system.h"
#include "bsp_my_uart.h"
#include "bsp_lock_drive.h"
#include "bsp_timer.h"
#include "bsp_battery_adc.h"
#include "bsp_common.h"

#include "app_user.h"
#include "app_comm_ble.h"
#include "app_comm_NB86.h"
#include "app_adv_ble.h"
#include "app_waes.h"
#include "app_common.h"

#include "task_safeguard.h"
#include "task_control.h"
#include "task_measure.h"
#include "task_mem_read.h"
#include "task_mem_write.h"
#include "task_bluetooth.h"
#include "task_communicate.h"
#include "task_other.h"
#include "task_common.h"

#endif

