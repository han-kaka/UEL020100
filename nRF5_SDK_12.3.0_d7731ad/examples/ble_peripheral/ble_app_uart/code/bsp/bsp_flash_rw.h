#ifndef  __BSP_FLASH_RW_H
#define  __BSP_FLASH_RW_H

#include "global.h"

///****************************************** Register data ***************************************************/
//#define ZHEJIANG                       0x0000

//#define HANGZHOU                       0x0000

//#define GPRS_ONLY                      0x01
//#define GPRS_AND_BLUETOOTH             0x02

//#define PROVINCE_ID                    ZHEJIANG
//#define CITY_ID                        HANGZHOU
//#define MANUFACTURER_ID                {0x01,0x01,0x01,0x01,0x01}
//#define TERMINAL_VERSION               {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}
//#define TERMINAL_TYPE                  GPRS_AND_BLUETOOTH
//#define TERMINAL_SOFTWARE_VER          {0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02}
//#define TERMINAL_HARDWARE_VER          {0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03}
///************************************************************************************************************/
//	
//	
#define SOLID_FLASH_DATA_PAGE                 2
#define LOG_INFO_DATA_PAGE                    3
//#define DYNAMIC_ROM_DATA_B_PAGE             4 
//#define SUPER_DYNAMIC_ROM_DATA_A_PAGE       3
//#define SUPER_DYNAMIC_ROM_DATA_B_PAGE       2 




//#define OPEN_LOCK_PWD_LEN                   6
//#define TIEM_LEN                            4
//#define AUTH_KEY_LEN                        10
//#define OPEN_LOCK_NO_LEN                    2
//#define OPEN_LOCK_ORDER_LEN                 39 

#define MAX_SAVE_SIZE 80
#define ROMDATA_SAVE_BYTES  MAX_SAVE_SIZE*4

///****************************************** solid data ****************************************************/
//typedef struct
//{	
//		uint16_t                           province_id;
//		uint16_t                           city_id;
//    uint8_t                            manufacturer_id[5];
//    char                               terminal_version[20];
//		uint8_t                            terminal_type;
//    uint8_t                            iccid[10];
//		char                               akey[AUTH_KEY_LEN];
//		char                               terminal_software[20];
//		char                               terminal_hardware[20];
//		uint8_t                            bluetooth_addr[7];
//}Solid_Register_Data_Type;

//typedef struct
//{	
//		char                               server_apn[6+1];
//    char                               server_ip[16+1];
//    uint32_t                           server_port;
//		uint32_t                           location_off_time;
//		uint32_t                           location_on_time;
//		uint32_t                           heartbeat_time;
//		uint32_t                           on_valid_time;
//		uint8_t                            start_time[7];
//    char                               net_type[5];	
//}Solid_Parameters_Data_Type;

typedef struct
{	
//		uint8_t                            ee_addr_init;
//		uint8_t                            ee_addr_test;
//		uint8_t                            ee_addr_power;
//		uint16_t                           ee_addr_poweroncnt;
		uint8_t                            ee_addr_timestamp[8];
//		uint8_t                            ee_addr_openlockdir;
		uint8_t                            ee_addr_aes128key[AES_KEY_LEN];
//		uint8_t                            ee_addr_lockname[16];
//		uint8_t                            ee_addr_lockindex[5];
		uint8_t                            ee_addr_imei[IMEI_LEN];
		uint8_t                            ee_addr_iccid[ICCID_LEN];
		uint8_t                            ee_addr_ip[SERVER_IP_LEN];
		uint8_t                            ee_addr_apn[SERVER_APN_LEN];
		uint8_t                            ee_addr_logindex[LOG_INDEX_LEN];
		uint8_t                            ee_addr_open_lock_dir;
	
}Solid_Data_Cell_Data_Type;

typedef struct
{	
		uint8_t                            writed1;
		uint8_t                            writed2;
		Solid_Data_Cell_Data_Type          solid_data_cell_data_struct;
		uint8_t                            solid_data_cell_xor;
}Solid_Data_Cell_Type;

///************************************************************************************************************/

///****************************************** dynamic data ****************************************************/
//typedef struct
//{
//		uint8_t                            lock_work_mode_flag   :3;   //00:正常模式 01:运输模式 02:锁重启
//		uint8_t                            c1100_mode_flag       :3;   //00:PSM 01:EDRX中的DRX不开启sleep 02:EDRX中的DRX开启sleep 03~07:EDRX
//		uint8_t                            reserved              :2;
//}Dynamic_Lock_State_Type;

//typedef struct
//{
//		uint8_t                            last_open_lock_mode;
//		char                               open_lock_data[OPEN_LOCK_ORDER_LEN+1];
//		uint32_t                           open_lock_time[TIEM_LEN];
//		uint8_t                            last_close_lock_time[TIEM_LEN];
//		char                               open_lock_pwd[OPEN_LOCK_PWD_LEN];
//		uint8_t                            open_lock_no[OPEN_LOCK_NO_LEN];
//		uint8_t                            open_lock_order[OPEN_LOCK_ORDER_LEN+1];
//}Dynamic_Lock_Data_Type;

//typedef struct
//{
//		uint8_t                            writed1;
//		uint8_t                            writed2;	
//		uint8_t                            num;	
//		Dynamic_Lock_State_Type            dynamic_lock_state_struct;
//		uint8_t                            dynamic_lock_state_xor;
//		Dynamic_Lock_Data_Type             dynamic_lock_data_struct;
//		uint8_t                            dynamic_lock_data_xor;
//}Dynamic_Data_Cell_Type;

typedef struct
{
		uint8_t  flag; // =0xA5有效
		uint8_t  action; // 执行的动作；0x01-密码；0x02-指纹；0x03-RFID；0x04-蓝牙钥匙；0x05-三次密码输入错误；0x06-指静脉；0x07-居民身份证；
		uint8_t  res[2];
		uint32_t time;
		uint8_t  userId[8];
		uint8_t  data[16];
} LogInfo_Type;

/************************************************************************************************************/
typedef union 
{
		uint8_t                            byte[ROMDATA_SAVE_BYTES];
		uint32_t                           data[MAX_SAVE_SIZE];		
		Solid_Data_Cell_Type               solid_data_cell_struct;          //固态存储
//		Dynamic_Data_Cell_Type             dynamic_data_cell_struct;        //动态存储

}Rom_Data_Type;

typedef struct
{
	uint8_t  flag; // bit7-用户；bit6-键盘密码；bit5-指纹；bit4-RFID；bit3-蓝牙钥匙；bit2-指静脉；bit1=1；bit0=0；
	uint16_t fingerId;
	uint8_t  userId[8];
	uint8_t  password[8];
	uint8_t  cardId[8];
	uint32_t startTime;
	uint32_t endTime;
	uint8_t  res[25];
	uint32_t validity; // 此结构体数据的有效性，0xAAAAAAAA-表示有效；
} UserInfo_t;

typedef struct
{
	uint8_t  flag; //=0xA5有效
	uint16_t head;
	uint16_t tail;
} LogInfoIndex_t;

typedef struct
{
	uint8_t  flag; //=0xA5有效
	uint8_t  action; //执行的动作；0x01-密码；0x02-指纹；0x03-RFID；0x04-蓝牙钥匙；0x05-三次密码输入错误；0x06-指静脉；0x07-居民身份证；
	uint8_t  res[2];
	uint32_t time;
	uint8_t  userId[8];
	uint8_t  data[16];
} LogInfo_t;

typedef Rom_Data_Type*                 p_Rom_Data_Type;

//void Read_Romdata(const uint32_t pg_num, p_Rom_Data_Type p_rom_data_struct);
//void Write_Romdata(const uint32_t pg_num, const p_Rom_Data_Type p_rom_data_struct);

/************************************** solid romdata *******************************************************/
void init_solid_flash_data(void);
bool store_solid_flsh_data(void);
bool Write_Solid_Romdata(const uint32_t pg_num, const p_Rom_Data_Type p_rom_data_struct);
//void init_register_data(void);
//void init_param_data(void);
//void init_aes_key_data(void);
/************************************************************************************************************/

///************************************** dynamic romdata *****************************************************/
//void init_dynamic_romdata(void);
//bool store_dynamic_romdata(void);
//void init_lock_state_data(void);
//bool check_lock_state_data(p_Rom_Data_Type p_dynamic_data_struct);
//void init_lock_data_data(void);
//bool check_lock_data_data(p_Rom_Data_Type p_dynamic_data_struct);
//void get_dynamic_one_flash(p_Rom_Data_Type p_dynamic_data_struct);
//void get_dynamic_two_flash(p_Rom_Data_Type p_dynamic_data_1_struct, p_Rom_Data_Type p_dynamic_data_2_struct);
//bool store_dynamic_flash(const uint32_t pg_num, const p_Rom_Data_Type p_dynamic_data_struct);
///************************************************************************************************************/

//void init_super_dynamic_romdata(void);

//uint8 UserSaveLogInfo(LogInfo_t *pBuf);
void read_log_data(Rom_Data_Type *p_log_data_struct);
uint8_t UserGetLogInfo(LogInfo_t *pBuf);
uint8_t UserDelLogInfo(LogInfo_t *pBuf);
//uint8 UserSaveUserInfo(uint8 num, UserInfo_t *pBuf);
//#ifdef FLASHUSERINFO
//	uint8 UserDelUserInfo(uint8 num);
//#endif
uint8_t UserGetUserInfo(uint8_t num, UserInfo_t *pBuf);
//uint8 UserSaveUserBluetoothKeyInfo(uint8 num, UserBluetoothKeyInfo_t *pBuf);
//uint8 UserGetUserBluetoothKeyInfo(uint8 num, UserBluetoothKeyInfo_t *pBuf);
//uint8 UserSaveAppData(EepromAddr_t parameter, uint8 *pbuf);
//uint8 UserGetAppData(EepromAddr_t parameter, uint8 *pbuf);
//uint8 SaveSetup(void);
//uint8 GetSetup(void);
//uint8 UserEepromInit(void);

#endif
