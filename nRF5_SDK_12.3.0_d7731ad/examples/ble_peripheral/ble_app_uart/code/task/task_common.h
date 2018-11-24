#ifndef __TASK_COMMON_H
#define __TASK_COMMON_H

#include "global.h"

#include "task_safeguard.h"
#include "task_control.h"
#include "task_measure.h"
#include "task_mem_read.h"
#include "task_mem_write.h"
#include "task_bluetooth.h"
#include "task_communicate.h"
#include "task_other.h"

#include "bsp_common.h"
#include "app_common.h"

#define TASK0                         SG_Task
#define TASK1                         Contrl_Task
#define TASK2                         Measure_Task
#define TASK3                         MEM_Read_Task
#define TASK4                         MEM_Write_Task
#define TASK5                         BlueTooth_Task
#define TASK6                         COMM_Task
#define TASK7                         Other_Task


#define SG                            0                             //ϵͳ������0 
#define SG_ENTER_NORMAL               1                             //�л�������ģʽ����1
#define SG_ENTER_TRANS                2                             //�л�������ģʽ����2

#define CONTROL                       1                             //��������1
#define CONTROL_MOTOR_OPEN            0                             //���ÿ�����0           
#define CONTROL_MOTOR_CLOSE           1                             //���ù�����1
#define CONTROL_VALVE_OPEN            2                             //������������0           
#define CONTROL_VALVE_CLOSE           3                             //������������1
#define CONTROL_POWER_OPEN            4
#define CONTROL_POWER_CLOSE           5

#define MEASURE                       2                             //��������2
#define MEASURE_CMP                   0                             //��ѹ�����������0
#define MEASURE_VOLTAGE               1                             //��ѹ�������1

#define MEM_READ                      3                             //
#define MEM_READ_SOLID_ROMDATA        0
#define MEM_SYS_PAR_INFO_RECOVER      1
#define MEM_VALVE_STATE_RECOVER       2
#define MEM_RUN_STATE_RECOVER         3
#define MEM_SYS_TEMP_AND_HUD_RECOVER  4
#define MEM_SYS_FAULT_STATE_RECOVER   5
#define MEM_SYS_RTC_RECOVER           6

#define MEM_WRITE                     4                             //���ݴ洢����4
#define MEM_STORE_SOLID_ROMDATA       0                             //�̶����ݴ洢����0
#define MEM_STORE_DYNAMIC_ROMDATA     1                             //��̬���ݴ洢����1

#define BLUETOOTH                     5                             //��������5
#define BLUETOOTH_PARSE_PROTOCOL      0                             //����Э���������0
#define BLUETOOTH_CRC_CHECK           1                             //����crcУ������1
#define BLUETOOTH_AES_DECODE          2                             //���ݽ�������2
#define BLUETOOTH_PROCESS_CMD         3                             //����ָ������3

#define COMM                          6                             //NBͨѶ����6  
#define COMM_CMD_PROC                 0
#define COMM_STATE_PROC               1
#define COMM_DECODE                   2
#define COMM_PC_DECODE		          	3
#define COMM_PC_RETURN  		      		4
#define COMM_CHECK                    5

#define COMM_G510_DECODE              5
#define COMM_G510_RETURN  		      	6
#define COMM_G510_UPLOAD		      		7

#define OTHER                         7                             //��������



typedef struct
{
    uint8_t (*function)(uint8_t m_Event);                                     //�������ĺ���ָ�� ����ִ��������
}Task_Type;

extern uint8_t                            g_Maintask;                   //ϵͳ������
extern uint8_t                            ga_Subtask[8];                //ϵͳ������
extern Task_Type                          Task_Struct[8];
extern const uint8_t                      ga_TaskMapTable[256];

#endif

