#include "app_util_platform.h"
#include "ble_gap.h"

//�û�ͷ�ļ�
#include "bsp_flash_rw.h"

/************************************** flash wr re *********************************************************/
static void Read_Romdata(const uint32_t pg_num, p_Rom_Data_Type p_rom_data_struct)
{  
		uint32_t i;
		uint32_t *addr;   
		uint32_t pg_size;
   
		pg_size = NRF_FICR->CODEPAGESIZE;  //ÿһҳ�Ĵ�С

	  // Start address:
    addr = (uint32_t *)(pg_size * pg_num);//����ҳ��ʼ��ַ
//		setWDI();
		for (i=0;i<MAX_SAVE_SIZE;i++)//ѭ��д��Ҫ�������
    {
				p_rom_data_struct->data[i]=*addr; 
				addr++;  //���Զ�����ɼ�4
    }
}

static void Write_Romdata(const uint32_t pg_num, const p_Rom_Data_Type p_rom_data_struct)
{  
		uint32_t        err_code;	
		uint32_t        *p_dst;
		uint32_t        *p_src;
		uint32_t   			pg_size;	
	
    pg_size = NRF_FICR->CODEPAGESIZE;  //ÿһҳ�Ĵ�С	
		p_dst = (uint32_t *)(pg_size * pg_num);//���㵹����6ҳ����ʼ��ַ	
	
		err_code = sd_flash_page_erase(pg_num);
		APP_ERROR_CHECK(err_code);	
//		sys_ndelay(4);
		p_src = (uint32_t *)p_rom_data_struct->data;
		sd_flash_write(p_dst, p_src, MAX_SAVE_SIZE);
//		sys_ndelay(4);
		__NOP();
}
/************************************************************************************************************/


/************************************** solid romdata *******************************************************/
bool Write_Solid_Romdata(const uint32_t pg_num, const p_Rom_Data_Type p_solid_data_struct)
{
		Rom_Data_Type   rom_data_temp_struct;	
		p_Rom_Data_Type p_rom_data_temp_struct = &rom_data_temp_struct;
	
//		Write_Romdata(pg_num, p_solid_data_struct);
//		Read_Romdata(pg_num, p_rom_data_temp_struct);
//	
//		if((p_rom_data_temp_struct->solid_data_cell_struct.writed1 == 0xaa)
//			 && (p_rom_data_temp_struct->solid_data_cell_struct.writed2 == 0xaa)
//			 && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->solid_data_cell_struct.solid_register_data_struct)),(sizeof(Solid_Register_Data_Type)+1)) == true) 
//			 && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->solid_data_cell_struct.solid_param_data_struct)),(sizeof(Solid_Parameters_Data_Type)+1)) == true) 
//			 && (Xor_Check(p_rom_data_temp_struct->solid_data_cell_struct.aes_key,(AES_KEY_LEN+1)) == true)
//			)
//		{
//		#if SEGGER_RTT_DEBUG_FLASH	
//				SEGGER_RTT_printf(0, "store solid rom data true!\r\n");
//		#endif
//				return true;
//		}
#if SEGGER_RTT_DEBUG_FLASH
		SEGGER_RTT_printf(0, "store solid rom data false!\r\n");
#endif
		return false;
}

bool Store_Solid_Romdata(void)
{
		uint32_t        pg_num;
		Rom_Data_Type   solid_data_struct;
		p_Rom_Data_Type p_solid_data_struct = &solid_data_struct;
	
	#if SEGGER_RTT_DEBUG_FLASH
		SEGGER_RTT_printf(0, "store solid rom data!\r\n");
	#endif
		pg_num = NRF_FICR->CODESIZE - SOLID_ROM_DATA_PAGE;  // Use last 5 page in flash ҳ����
	
		p_solid_data_struct->solid_data_cell_struct.writed1 = 0xaa;
		p_solid_data_struct->solid_data_cell_struct.writed2 = 0xaa;
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.ee_addr_aes128key, useraeskeybuf, AES_KEY_LEN);
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.ee_addr_imei, NB_CommPacket.AuthData.imei, IMEI_LEN);
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.ee_addr_iccid, NB_CommPacket.AuthData.iccid, ICCID_LEN);
		
		memcpy(p_solid_data_struct->solid_data_cell_struct.ee_addr_ip, NB_NetPar.ServerIp, SERVER_IP_LEN);
		
		memcpy(p_solid_data_struct->solid_data_cell_struct.ee_addr_apn, NB_NetPar.ServerApn, SERVER_APN_LEN);
	
		p_solid_data_struct->solid_data_cell_struct.solid_data_cell_xor = Get_Xor((uint8_t*)(&solid_data_struct), sizeof(Rom_Data_Type));
		
		return Write_Solid_Romdata(pg_num, p_solid_data_struct);
}

//void init_solid_romdata(void)
//{ 
//		uint32_t        pg_num;
//		Rom_Data_Type   solid_data_struct;
//		p_Rom_Data_Type p_solid_data_struct = &solid_data_struct;

//		pg_num = NRF_FICR->CODESIZE - SOLID_ROM_DATA_PAGE;  // Use last 5 page in flash ҳ����

//		Read_Romdata(pg_num, p_solid_data_struct);
//	
//		if((p_solid_data_struct->solid_data_cell_struct.writed1 == 0xaa) || (p_solid_data_struct->solid_data_cell_struct.writed2 == 0xaa))
//		{
//				//register data
//				if(Xor_Check((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_register_data_struct)),(sizeof(Solid_Register_Data_Type)+1)) == true)
//				{
//						memcpy(&Register_Package_Struct,(uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_register_data_struct)),sizeof(Register_Package_Type));
//						NRF_LOG_DIRECT("get register data!\r\n");
//				}
//				else
//				{
//						NRF_LOG_DIRECT("init register data!\r\n");
//						init_register_data();
//						Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
//				}
//				
//				//param data
//				if(Xor_Check((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_param_data_struct)),(sizeof(Solid_Parameters_Data_Type)+1)) == true)
//				{
//						memcpy(&Net_Parameters_Struct,(uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_param_data_struct)),sizeof(Net_Parameters_Type));
//						NRF_LOG_DIRECT("get param data!\r\n");
//				}
//				else
//				{
//						NRF_LOG_DIRECT("init param data!\r\n");
//						init_param_data();
//						Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
//				}

//				//aes key data
//				if(Xor_Check(p_solid_data_struct->solid_data_cell_struct.aes_key,(AES_KEY_LEN+1)) == true)
//				{
//						memcpy(AES_KEY, p_solid_data_struct->solid_data_cell_struct.aes_key, AES_KEY_LEN);
//						NRF_LOG_DIRECT("get aes key data!\r\n");
//				}					
//				else
//				{
//						NRF_LOG_DIRECT("init aes key data!\r\n");
//						init_aes_key_data();
//						Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
//				}	
//		}
//		else
//		{
//				NRF_LOG_DIRECT("init solid rom data!\r\n");
//				init_register_data();
//				init_param_data();
//				init_aes_key_data();
//				Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
//		}
//}

//void init_register_data(void)
//{
//		uint32_t           err_code = 1;
//		ble_gap_addr_t     device_addr;	
//	
//		Register_Package_Struct.province_id       = PROVINCE_ID;
//		Register_Package_Struct.city_id           = CITY_ID;
//		Register_Package_Struct.manufacturer_id[0]= 0x01;
//		Register_Package_Struct.manufacturer_id[1]= 0x01;
//		Register_Package_Struct.manufacturer_id[2]= 0x01;
//		Register_Package_Struct.manufacturer_id[3]= 0x01;
//		Register_Package_Struct.manufacturer_id[4]= 0x01;
//		for(uint8_t i=0; i<20; i++)
//		{
//				Register_Package_Struct.terminal_version[i] = 0x02;
//		}
//		Register_Package_Struct.terminal_type     = TERMINAL_TYPE;
//		for(uint8_t i=0; i<19; i++)
//		{
//				Register_Package_Struct.terminal_software[i] = 0x03;
//		}
//        Register_Package_Struct.terminal_software[19] = 0x00;
//		for(uint8_t i=0; i<19; i++)
//		{
//				Register_Package_Struct.terminal_hardware[i] = 0x04;
//		}
//        Register_Package_Struct.terminal_hardware[19] = 0x00;
//		// Get BLE address.
//		#if (NRF_SD_BLE_API_VERSION == 3)
//				err_code = sd_ble_gap_addr_get(&device_addr);
//				NRF_LOG_DIRECT("get mac addr err_code = 0x%02X \r\n",err_code);
//		#else
//				err_code = sd_ble_gap_address_get(&device_addr);
//				NRF_LOG_DIRECT("get mac addr err_code = 0x%02X \r\n",err_code);
//		#endif
//		for(int i = 0;i<BLE_GAP_ADDR_LEN;i++)
//		{
//				Register_Package_Struct.bluetooth_addr[i] = device_addr.addr[BLE_GAP_ADDR_LEN-1-i];
//		}
//		Register_Package_Struct.bluetooth_addr[6] = 0x00;		
//}

//void init_param_data(void)
//{
//		strcpy(Net_Parameters_Struct.server_apn, "CMIOT");
//		strcpy(Net_Parameters_Struct.server_ip, "118.178.227.84");
//		Net_Parameters_Struct.server_port       = 20000;
//		Net_Parameters_Struct.location_off_time = SEARCH_GPS_TIMEOUT;
//		Net_Parameters_Struct.location_on_time  = SEARCH_GPS_TIMEOUT;
//		Net_Parameters_Struct.heartbeat_time    = VOLT_TEMP_TIMEOUT;
//		Net_Parameters_Struct.on_valid_time     = ON_VALID_TIMEOUT;
//	
//		Net_Parameters_Struct.start_time[0]     = 20;
//		Net_Parameters_Struct.start_time[1]     = 00;
//		Net_Parameters_Struct.start_time[2]     = 00;
//		Net_Parameters_Struct.start_time[3]     = 00;
//		Net_Parameters_Struct.start_time[4]     = 00;
//		Net_Parameters_Struct.start_time[5]     = 00;

//		strcpy(Net_Parameters_Struct.net_type, "TCP");
//}

//void init_aes_key_data(void)
//{
//		AES_KEY[0] = 0x3A;
//		AES_KEY[1] = 0x60;
//		AES_KEY[2] = 0x43;
//		AES_KEY[3] = 0x2A;
//		AES_KEY[4] = 0x5C;
//		AES_KEY[5] = 0x01;
//		AES_KEY[6] = 0x21;
//		AES_KEY[7] = 0x1F;
//		AES_KEY[8] = 0x29;
//		AES_KEY[9] = 0x1E;
//		AES_KEY[10] = 0x0F;
//		AES_KEY[11] = 0x4E;
//		AES_KEY[12] = 0x0C;
//		AES_KEY[13] = 0x13;
//		AES_KEY[14] = 0x28;
//		AES_KEY[15] = 0x25;
//}
///************************************************************************************************************/

///************************************** dynamic romdata *****************************************************/
//bool store_dynamic_romdata(void)
//{
//		uint32_t        pg_num_a;
//		uint32_t        pg_num_b;
//		Rom_Data_Type   dynamic_data_struct;
//		Rom_Data_Type   dynamic_data_a_struct;
//		Rom_Data_Type   dynamic_data_b_struct;
//		p_Rom_Data_Type p_dynamic_data_struct = &dynamic_data_struct;
//		p_Rom_Data_Type p_dynamic_data_a_struct = &dynamic_data_a_struct;
//		p_Rom_Data_Type p_dynamic_data_b_struct = &dynamic_data_b_struct;

//		NRF_LOG_DIRECT("store dynamic rom data!\r\n");
//	
//		p_dynamic_data_struct->dynamic_data_cell_struct.writed1 = 0xaa;
//		p_dynamic_data_struct->dynamic_data_cell_struct.writed2 = 0xaa;	
//	
//		memcpy((uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_state_struct)), &Sys_Lock_State_Struct, sizeof(Sys_Lock_State_Type));
//		p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_state_xor = Get_Xor((uint8_t*)(&Sys_Lock_State_Struct), sizeof(Sys_Lock_State_Type));
//	
//		memcpy((uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_data_struct)), &Sys_Lock_Data_Struct, sizeof(Sys_Lock_Data_Type));
//		p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_data_xor = Get_Xor((uint8_t*)(&Sys_Lock_Data_Struct), sizeof(Sys_Lock_Data_Type));

//        pg_num_a = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_A_PAGE;  // Use last 4 page in flash ҳ����
//        pg_num_b = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_B_PAGE;  // Use last 3 page in flash ҳ����

//		Read_Romdata(pg_num_a, p_dynamic_data_a_struct);
//		Read_Romdata(pg_num_b, p_dynamic_data_b_struct);

//		//ֻ�е�����4��flash������Ч����
//		if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			   &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 != 0xaa)))
//		{
//				p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_a_struct->dynamic_data_cell_struct.num+1;
//				return store_dynamic_flash(pg_num_b, p_dynamic_data_struct);
//		}
//		//ֻ�е�����3��flash������Ч����
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 != 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_b_struct->dynamic_data_cell_struct.num+1;
//				return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//		}
//		//����flash��������Ч����
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num == 0xff)
//				{
//						if(p_dynamic_data_b_struct->dynamic_data_cell_struct.num == 0x00)
//						{
//								p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_b_struct->dynamic_data_cell_struct.num+1;
//								return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//						}
//						else
//						{
//								p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_a_struct->dynamic_data_cell_struct.num+1;
//								return store_dynamic_flash(pg_num_b, p_dynamic_data_struct);
//						}
//				}
//				if(p_dynamic_data_b_struct->dynamic_data_cell_struct.num == 0xff)
//				{
//						if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num == 0x00)
//						{
//								p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_a_struct->dynamic_data_cell_struct.num+1;
//								return store_dynamic_flash(pg_num_b, p_dynamic_data_struct);
//						}
//						else
//						{
//								p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_b_struct->dynamic_data_cell_struct.num+1;
//								return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//						}
//				}

//				else if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num >= p_dynamic_data_b_struct->dynamic_data_cell_struct.num)
//				{
//						p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_a_struct->dynamic_data_cell_struct.num+1;
//						return store_dynamic_flash(pg_num_b, p_dynamic_data_struct);
//				}
//				else
//				{
//						p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_b_struct->dynamic_data_cell_struct.num+1;
//						return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//				}				
//		}
//		else
//		{
//				p_dynamic_data_struct->dynamic_data_cell_struct.num = 0x00;
//				return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//		}
//}

//bool store_dynamic_flash(const uint32_t pg_num, const p_Rom_Data_Type p_dynamic_data_struct)
//{
//		Rom_Data_Type   rom_data_temp_struct;	
//		p_Rom_Data_Type p_rom_data_temp_struct = &rom_data_temp_struct;
//	
//		Write_Romdata(pg_num, p_dynamic_data_struct);
//		Read_Romdata(pg_num, p_rom_data_temp_struct);
//	
//		if((p_rom_data_temp_struct->dynamic_data_cell_struct.writed1 == 0xaa)
//			 && (p_rom_data_temp_struct->dynamic_data_cell_struct.writed2 == 0xaa)
//			 && (p_rom_data_temp_struct->dynamic_data_cell_struct.num == p_dynamic_data_struct->dynamic_data_cell_struct.num)
//			 && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->dynamic_data_cell_struct.dynamic_lock_state_struct)),(sizeof(Sys_Lock_State_Type)+1)) == true) 
//			 && (Xor_Check((uint8_t*)(&(p_rom_data_temp_struct->dynamic_data_cell_struct.dynamic_lock_data_struct)),(sizeof(Sys_Lock_Data_Type)+1)) == true)
//			)
//		{
//				NRF_LOG_DIRECT("store dynamic rom data true!\r\n");
//				return true;
//		}
//		NRF_LOG_DIRECT("store dynamic rom data false!\r\n");
//		return false;
//}

//void init_dynamic_romdata(void)
//{
//		uint32_t        pg_num_a;
//		uint32_t        pg_num_b;
//		Rom_Data_Type   dynamic_data_a_struct;
//		Rom_Data_Type   dynamic_data_b_struct;
//		p_Rom_Data_Type p_dynamic_data_a_struct = &dynamic_data_a_struct;
//		p_Rom_Data_Type p_dynamic_data_b_struct = &dynamic_data_b_struct;

//    pg_num_a = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_A_PAGE;  // Use last 4 page in flash ҳ����
//    pg_num_b = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_B_PAGE;  // Use last 3 page in flash ҳ����

//		Read_Romdata(pg_num_a,p_dynamic_data_a_struct);
//		Read_Romdata(pg_num_b,p_dynamic_data_b_struct);

//		//ֻ�е�����4��flash������Ч����
//		if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			   &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 != 0xaa)))
//		{
//				get_dynamic_one_flash(p_dynamic_data_a_struct);
//		}
//		//ֻ�е�����3��flash������Ч����
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 != 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				get_dynamic_one_flash(p_dynamic_data_b_struct);
//		}
//		//����flash��������Ч����
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num == 0xff)
//				{
//						if(p_dynamic_data_b_struct->dynamic_data_cell_struct.num == 0x00)
//						{
//								get_dynamic_two_flash(p_dynamic_data_b_struct, p_dynamic_data_a_struct);
//						}
//						else
//						{
//								get_dynamic_two_flash(p_dynamic_data_a_struct, p_dynamic_data_b_struct);
//						}
//				}
//				if(p_dynamic_data_b_struct->dynamic_data_cell_struct.num == 0xff)
//				{
//						if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num == 0x00)
//						{
//								get_dynamic_two_flash(p_dynamic_data_a_struct, p_dynamic_data_b_struct);
//						}
//						else
//						{
//								get_dynamic_two_flash(p_dynamic_data_b_struct, p_dynamic_data_a_struct);
//						}
//				}

//				else if(p_dynamic_data_a_struct->dynamic_data_cell_struct.num >= p_dynamic_data_b_struct->dynamic_data_cell_struct.num)
//				{
//						get_dynamic_two_flash(p_dynamic_data_a_struct, p_dynamic_data_b_struct);
//				}
//				else
//				{
//						get_dynamic_two_flash(p_dynamic_data_b_struct, p_dynamic_data_a_struct);
//				}
//		}
//		//����flash��û����Ч����
//		else
//		{
//				NRF_LOG_DIRECT("init dynamic rom data!\r\n");
//				init_lock_state_data();
//				init_lock_data_data();
//				Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_ROMDATA);
//		}
//}

//void init_lock_state_data(void)
//{
//		
//		Sys_Lock_State_Struct.lock_work_mode_flag = LOCK_MODE_NORMAL;
//		Sys_Lock_State_Struct.c1100_mode_flag = C1100_MODE_DRX;
//}

//void init_lock_data_data(void)
//{
//		
//		strcpy(Sys_Lock_Data_Struct.open_lock_pwd, "000000");
//}

//bool check_lock_state_data(p_Rom_Data_Type p_dynamic_data_struct)
//{
//		//lock state data
//		if(Xor_Check((uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_state_struct)),(sizeof(Dynamic_Lock_State_Type)+1)) == true)
//		{
//				memcpy(&Sys_Lock_State_Struct, (uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_state_struct)), sizeof(Sys_Lock_State_Type));
//				NRF_LOG_DIRECT("get lock state data!\r\n");
//				return true;
//		}
//		return false;
//}

//bool check_lock_data_data(p_Rom_Data_Type p_dynamic_data_struct)
//{
//		//lock data
//		if(Xor_Check((uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_data_struct)),(sizeof(Dynamic_Lock_Data_Type)+1)) == true)
//		{
//				memcpy(&Sys_Lock_Data_Struct, (uint8_t*)(&(p_dynamic_data_struct->dynamic_data_cell_struct.dynamic_lock_data_struct)), sizeof(Sys_Lock_Data_Type));
//				NRF_LOG_DIRECT("get lock data!\r\n");
//				return true;
//		}
//		return false;
//}

//void get_dynamic_one_flash(p_Rom_Data_Type p_dynamic_data_struct)
//{
//		//lock state data
//		if(false == check_lock_state_data(p_dynamic_data_struct))
//		{
//                NRF_LOG_DIRECT("init lock state data!\r\n");
//				init_lock_state_data();
//				Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_ROMDATA);
//		}
//		
//		//lock data
//		if(false == check_lock_data_data(p_dynamic_data_struct))
//		{
//                NRF_LOG_DIRECT("init lock data!\r\n");
//				init_lock_data_data();
//				Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_ROMDATA);
//		}
//}

//void get_dynamic_two_flash(p_Rom_Data_Type p_dynamic_data_1_struct, p_Rom_Data_Type p_dynamic_data_2_struct)
//{
//		//lock state data
//		if(false == check_lock_state_data(p_dynamic_data_1_struct))
//		{
//				if(false == check_lock_state_data(p_dynamic_data_2_struct))
//				{
//                        NRF_LOG_DIRECT("init lock state data!\r\n");
//						init_lock_state_data();
//						Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_ROMDATA);
//				}
//		}
//		
//		//lock data
//		if(false == check_lock_data_data(p_dynamic_data_1_struct))
//		{
//				if(false == check_lock_data_data(p_dynamic_data_2_struct))
//				{
//                        NRF_LOG_DIRECT("init lock data!\r\n");
//						init_lock_data_data();
//						Set_Task(MEM_WRITE, MEM_STORE_DYNAMIC_ROMDATA);
//				}
//		}
//}
///************************************************************************************************************/


//void init_super_dynamic_romdata(void)
//{
//		
//}

