#ifndef __APP_COMMON_H
#define __APP_COMMON_H

#include "global.h"

//#include "app_user.h"
//#include "app_comm_ble.h"
//#include "app_comm_NB86.h"

//#include "bsp_common.h"
//#include "task_common.h"

#define TASK_ALL                       0xff


typedef struct
{		
		uint8_t                             reserved               :5;
}Sys_Flag_Type;






void set_task(uint8_t main_task, uint8_t sub_task);
void Clr_Task(uint8_t main_task, uint8_t sub_task);
uint8_t clear_task(uint8_t prio, uint8_t m_SYS_SubTask_prio);
//u8 BCD_2_Data8(u8 data);
//u8 Data8_2_BCD(u8 data);
//u16 Data16_2_BCD(u16 data);
//void Data32_2_BCD(u8* scr,u8* obj);

bool Check_Strstr(char *data1, char *data2, uint16_t len);
uint8_t CntLeadZeros(uint8_t ucData);
uint8_t Get_Symbol(uint8_t num_symbol, uint8_t *buf, char symbol);
uint8_t Get_Comma(uint8_t num_comma, uint8_t *buf);
//u8* DecimalToASCII(u16 port,u8*data);
uint8_t Get_Xor(uint8_t *hex_data_buf, uint16_t len_hex);
bool Xor_Check(uint8_t *hex_data_buf,uint16_t len_hex);
uint8_t Get_Uint_Data(const char *buf);
uint16_t Ascii_To_Hex(char *ascs,uint8_t *hexs,uint16_t length);
#endif

