#ifndef __TASK_BLUETOOTH_H
#define __TASK_BLUETOOTH_H

#include "global.h"

#define Nb	4							// 以“字”（4字节）为单位的加解密数据块大小，固定为4

uint8_t BlueTooth_Task(uint8_t prio);

#endif



