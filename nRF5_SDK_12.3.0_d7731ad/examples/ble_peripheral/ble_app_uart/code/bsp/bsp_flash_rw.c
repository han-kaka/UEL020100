#include "app_util_platform.h"
#include "ble_gap.h"

//用户头文件
#include "bsp_flash_rw.h"

/************************************** flash wr re *********************************************************/
static void read_flash_data(const uint32_t pg_num, p_Rom_Data_Type p_rom_data_struct)
{  
		uint32_t i;
		uint32_t *addr;   
		uint32_t pg_size;
   
		pg_size = NRF_FICR->CODEPAGESIZE;  //每一页的大小

	  // Start address:
    addr = (uint32_t *)(pg_size * pg_num);//计算页起始地址
//		setWDI();
		for (i=0;i<MAX_SAVE_SIZE;i++)//循环写入要保存的字
    {
				p_rom_data_struct->data[i]=*addr; 
				addr++;  //会自动编译成加4
    }
}

static void write_flash_data(const uint32_t pg_num, const p_Rom_Data_Type p_rom_data_struct)
{  
		uint32_t        err_code;	
		uint32_t        *p_dst;
		uint32_t        *p_src;
		uint32_t   			pg_size;	
	
    pg_size = NRF_FICR->CODEPAGESIZE;  //每一页的大小	
		p_dst = (uint32_t *)(pg_size * pg_num);//计算倒数第6页的起始地址	
	
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
bool write_solid_flash_data(const uint32_t pg_num, const p_Rom_Data_Type p_solid_data_struct)
{
		Rom_Data_Type   rom_data_temp_struct;	
		p_Rom_Data_Type p_rom_data_temp_struct = &rom_data_temp_struct;
	
		write_flash_data(pg_num, p_solid_data_struct);
		read_flash_data(pg_num, p_rom_data_temp_struct);
	
		if((p_rom_data_temp_struct->solid_data_cell_struct.writed1 == 0xaa)
			 && (p_rom_data_temp_struct->solid_data_cell_struct.writed2 == 0xaa)
			 && (true == xor_check((uint8_t*)(&(p_rom_data_temp_struct->solid_data_cell_struct.solid_data_cell_data_struct)),(sizeof(Solid_Data_Cell_Data_Type)+1)))
			)
		{
		#if SEGGER_RTT_DEBUG_FLASH	
				SEGGER_RTT_printf(0, "store solid rom data true!\r\n");
		#endif
				return true;
		}
#if SEGGER_RTT_DEBUG_FLASH
		SEGGER_RTT_printf(0, "store solid rom data false!\r\n");
#endif
		return false;
}

//保存固态数据
bool store_solid_flsh_data(void)
{
		uint32_t        pg_num;
		Rom_Data_Type   solid_data_struct;
		p_Rom_Data_Type p_solid_data_struct = &solid_data_struct;
	
	#if SEGGER_RTT_DEBUG_FLASH
		SEGGER_RTT_printf(0, "store solid rom data!\r\n");
	#endif
		pg_num = NRF_FICR->CODESIZE - SOLID_FLASH_DATA_PAGE;  // Use last 5 page in flash 页数量
	
		p_solid_data_struct->solid_data_cell_struct.writed1 = 0xaa;
		p_solid_data_struct->solid_data_cell_struct.writed2 = 0xaa;
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_timestamp, timestamp, 8);
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_aes128key, useraeskeybuf, AES_KEY_LEN);
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_imei, NB_CommPacket.Init_Data.imei, IMEI_LEN);
	
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_iccid, NB_CommPacket.Init_Data.iccid, ICCID_LEN);
		
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_ip, NB_NetPar.ServerIp, SERVER_IP_LEN);
		
		memcpy(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_apn, NB_NetPar.ServerApn, SERVER_APN_LEN);
	
		p_solid_data_struct->solid_data_cell_struct.solid_data_cell_xor = get_xor((uint8_t*)(&solid_data_struct), sizeof(Solid_Data_Cell_Data_Type));
		
		return write_solid_flash_data(pg_num, p_solid_data_struct);
}

///*******************************************************************************
// * @brief       保存日志信息
// * @param       num：用户编号；
// * @return      0-错误；1-正确；
// *******************************************************************************
// */
//uint8 UserSaveLogInfo(LogInfo_t *pBuf)
//{
//	LogInfoIndex_t tmpLogInfoIndex;
//	
//	UserAT24C64Read(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
//	if (tmpLogInfoIndex.flag != 0xAA)
//	{
//		tmpLogInfoIndex.flag = 0xAA;
//		tmpLogInfoIndex.head = 0;
//		tmpLogInfoIndex.tail = 0;
//	}
//	
//	UserAT24C64Write(EE_ADDR_LOGINFO + sizeof(LogInfo_t) * tmpLogInfoIndex.head, 
//							sizeof(LogInfo_t), (uint8 *)pBuf);
//	if (++tmpLogInfoIndex.head >= MAX_LOG_NUM) tmpLogInfoIndex.head = 0;
//	if (tmpLogInfoIndex.head == tmpLogInfoIndex.tail) tmpLogInfoIndex.tail++;
//	if (tmpLogInfoIndex.tail >= MAX_LOG_NUM) tmpLogInfoIndex.tail = 0;
//	
//	ChangeAdvData(5, 1); // 更新广播标志位
//	#ifdef NBMODULE
//	UserNBModuleSetTodoFlag(0x01);
//	#endif
//	
//	return UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
//}

/*******************************************************************************
 * @brief       读取日志信息
 * @param       num：用户编号；
 * @return      0-错误；1-正确；
 *******************************************************************************
 */
uint8_t UserGetLogInfo(LogInfo_t *pBuf)
{
		LogInfoIndex_t tmpLogInfoIndex;
	
//	UserAT24C64Read(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
		if (tmpLogInfoIndex.flag != 0xAA)
		{
				tmpLogInfoIndex.flag = 0xAA;
				tmpLogInfoIndex.head = 0;
				tmpLogInfoIndex.tail = 0;
//				UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
		}
//	if (tmpLogInfoIndex.head == tmpLogInfoIndex.tail) 
//	{
//		ChangeAdvData(5, 0); // ???????
//		return 0;
//	}
//	else
//	{
//		ChangeAdvData(5, 1); // ???????
//	}
//	
//	if (pBuf != 0)
//	UserAT24C64Read(EE_ADDR_LOGINFO + sizeof(LogInfo_t) * tmpLogInfoIndex.tail, 
//							sizeof(LogInfo_t), (uint8 *)pBuf);
	return 1;
}

/*******************************************************************************
 * @brief       删除日志信息
 * @param       pBuf - 日志信息，若地址为0怎删除最老的一条日志信息
 * @return      0-错误；1-正确；
 *******************************************************************************
 */
uint8_t UserDelLogInfo(LogInfo_t *pBuf)
{
	LogInfoIndex_t tmpLogInfoIndex;
	LogInfo_t tmpLogInfo;
	
//	UserAT24C64Read(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
	if (tmpLogInfoIndex.flag != 0xAA)
	{
		tmpLogInfoIndex.flag = 0xAA;
		tmpLogInfoIndex.head = 0;
		tmpLogInfoIndex.tail = 0;
//		UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
	}
//	if (tmpLogInfoIndex.head == tmpLogInfoIndex.tail) 
//	{
//		ChangeAdvData(5, 0); // 更新广播标志位
//		return 0;
//	}
//	else
//	{
//		ChangeAdvData(5, 1); // 更新广播标志位
//	}
	
	if (pBuf > 0)
	{
//		UserAT24C64Read(EE_ADDR_LOGINFO + sizeof(LogInfo_t) * tmpLogInfoIndex.tail, 
//								sizeof(LogInfo_t), (uint8 *)(&tmpLogInfo));
		if ((memcmp(tmpLogInfo.userId , pBuf->userId, sizeof(tmpLogInfo.userId)) != 0) && 
			(tmpLogInfo.time == pBuf->time))
		{
			if (++tmpLogInfoIndex.tail >= MAX_LOG_NUM) tmpLogInfoIndex.tail = 0;
//			UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
		}
	}
	else
	{
		if (++tmpLogInfoIndex.tail >= MAX_LOG_NUM) tmpLogInfoIndex.tail = 0;
//		UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
	}
	return 1;
}

///*******************************************************************************
// * @brief       保存用户信息
// * @param       num：用户编号；
// * @return      0-成功；0xFF-未完成；other-失败；
// *******************************************************************************
// */
//#ifdef FLASHUSERINFO
//uint8 UserSaveUserInfo(uint8 num, UserInfo_t *pBuf)
//{
//	if (num >= MAX_USER_NUM) return 1;
//	pBuf->validity = 0xAAAAAAAA;
//	return UserFlashWrite(USER_FLASH_PAGE_USERINFO_START + (num / (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t))), 
//						  (uint16)(num % (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t))) * sizeof(UserInfo_t), 
//						  (uint8*)pBuf, sizeof(UserInfo_t));
//}
//#else
//uint8 UserSaveUserInfo(uint8 num, UserInfo_t *pBuf)
//{
//	uint8  ret = 1;
//	
//	if (num >= MAX_USER_NUM) return 1;
//	if (UserAT24C64Write(EE_ADDR_USERINFO + sizeof(UserInfo_t) * num, 
//							sizeof(UserInfo_t), (uint8 *)pBuf) == 1)
//		ret = 0;
//	return ret;
//}
//#endif
///*******************************************************************************
// * @brief       删除用户信息
// * @param       num：用户编号；
// * @return      0-成功；0xFF-未完成；other-失败；
// *******************************************************************************
// */
//#ifdef FLASHUSERINFO
//uint8 UserDelUserInfo(uint8 num)
//{
//	uint8  buf[4] = {0, 0, 0, 0};
//	uint16 addr = (((uint16)(num % (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t))) * sizeof(UserInfo_t) + sizeof(UserInfo_t) - sizeof(uint32)) >> 2) + 
//		(((uint16)USER_FLASH_PAGE_USERINFO_START + ((uint16)num / (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t)))) << 9);
//	
//	if (num >= MAX_USER_NUM) return 1;
//	HalFlashWrite(addr,  buf, 1);
//	return 0;
//}
//#endif

/*******************************************************************************
 * @brief       读取用户信息
 * @param       num：用户编号；
 * @return      0-成功；0xFF-未完成；other-失败；
 *******************************************************************************
 */
#ifdef FLASHUSERINFO
uint8_t UserGetUserInfo(uint8_t num, UserInfo_t *pBuf)
{
	uint8  ret;
	
//	if (num >= MAX_USER_NUM) return 1;
//	ret = UserFlashRead(USER_FLASH_PAGE_USERINFO_START + (num / (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t))), 
//						(uint16)(num % (USER_FLASH_PAGE_SIZE / sizeof(UserInfo_t))) * sizeof(UserInfo_t), 
//						(uint8*)pBuf, sizeof(UserInfo_t));
//	if (ret != 0xFF)
//	{
//		if (pBuf->validity != 0xAAAAAAAA)
//		{
//			osal_memset(pBuf, 0, sizeof(UserInfo_t));
//		}
//	}
	return ret;
}
#else
uint8_t UserGetUserInfo(uint8_t num, UserInfo_t *pBuf)
{
	uint8_t  ret = 1;
	
//	if (num >= MAX_USER_NUM) return 1;
//	if (UserAT24C64Read(EE_ADDR_USERINFO + sizeof(UserInfo_t) * num, 
//						   sizeof(UserInfo_t), (uint8 *)pBuf) == 1)
//		ret = 0;
	return ret;
}
#endif

///*******************************************************************************
// * @brief       保存用户蓝牙钥匙
// * @param       num：用户编号；
// * @return      0-错误；1-正确；
// *******************************************************************************
// */
//uint8 UserSaveUserBluetoothKeyInfo(uint8 num, UserBluetoothKeyInfo_t *pBuf)
//{
//	if (num >= MAX_USER_NUM) return 0;
//	return UserAT24C64Write(EE_ADDR_BLUETOOTH_KEY + sizeof(UserBluetoothKeyInfo_t) * num, 
//							sizeof(UserBluetoothKeyInfo_t), (uint8 *)pBuf);
//}

///*******************************************************************************
// * @brief       读取用户蓝牙钥匙
// * @param       num：用户编号；
// * @return      0-错误；1-正确；
// *******************************************************************************
// */
//uint8 UserGetUserBluetoothKeyInfo(uint8 num, UserBluetoothKeyInfo_t *pBuf)
//{
//	if (num >= MAX_USER_NUM) return 0;
//	return UserAT24C64Read(EE_ADDR_BLUETOOTH_KEY + sizeof(UserBluetoothKeyInfo_t) * num, 
//						   sizeof(UserBluetoothKeyInfo_t), (uint8 *)pBuf);
//}

////==============================================================================
////                           * 锁的设置参数 *
////==============================================================================
//uint8 SaveSetup(void)															// 保存设置到 EEPROM
//{
//	uint8  tmp;
//	
//	tmp = UserSaveAppData(P_EE_ADDR_INIT, &gSystemRunParam.flagInit);			// 配置改变时需要与 EEPROM 同步
//	if (gSystemRunParam.flagInit != 0xA5)
//	{
//		osal_memcpy(useraeskeybuf, defualtaes128key, 16);
//	}
//	if (gSystemRunParam.flagInit == 0xA5)
//	{
//		Save_SysRunState(2);
//	}
//	ChangeAdvData(2, gSystemRunParam.flagInit);									// 更新广播数据
//	
//	return tmp;
//}

//uint8 GetSetup(void)															// 从 EEPROM 获取配置
//{
//    uint8 tmp;
//	
//	tmp	= UserGetAppData(P_EE_ADDR_INIT, &gSystemRunParam.flagInit);
//    if ((gSystemRunParam.flagInit != 0xA5) && (gSystemRunParam.flagInit != 0x55))
//	{ gSystemRunParam.flagInit = 0; SaveSetup(); }								// 判断设置信息无效时，然后将其清零重新同步到 EEPROM 
//    else 
//	{ ChangeAdvData(2, gSystemRunParam.flagInit); }								// 否则改变广播状态
//	
//	return tmp;
//}

////==============================================================================
////                           * 锁的设置参数 *
////==============================================================================
//uint8 UserEepromInit(void)
//{
////	LogInfoIndex_t tmpLogInfoIndex;
////	
////	tmpLogInfoIndex.flag = 0xA0;
////	tmpLogInfoIndex.head = 0;
////	tmpLogInfoIndex.tail = 0;
////	return UserAT24C64Write(EE_ADDR_LOGINDEX, sizeof(LogInfoIndex_t), (uint8 *)&tmpLogInfoIndex);
//	return 1;
//}

//初始化固态数据
void init_solid_flash_data(void)
{
		uint32_t        pg_num;
		Rom_Data_Type   solid_data_struct;
		p_Rom_Data_Type p_solid_data_struct = &solid_data_struct;

		pg_num = NRF_FICR->CODESIZE - SOLID_FLASH_DATA_PAGE;  // Use last 2 page in flash 页数量

		read_flash_data(pg_num, p_solid_data_struct);
	
		if((p_solid_data_struct->solid_data_cell_struct.writed1 == 0xaa) &&
				(p_solid_data_struct->solid_data_cell_struct.writed2 == 0xaa) &&
				(true == xor_check((uint8_t*)(&(p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct)),(sizeof(Solid_Data_Cell_Data_Type)+1))))
		{
		#if SEGGER_RTT_DEBUG_FLASH
				SEGGER_RTT_printf(0, "mencpy solid flash data!\r\n");
		#endif
				memcpy(timestamp, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_timestamp,
								8);
			
				memcpy(useraeskeybuf, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_aes128key, 
								AES_KEY_LEN);
			
				memcpy(NB_CommPacket.Init_Data.imei, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_imei, 
								IMEI_LEN);
			
				memcpy(NB_CommPacket.Init_Data.iccid, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_iccid, 
								ICCID_LEN);
				
				memcpy(NB_NetPar.ServerIp, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_ip, 
								SERVER_IP_LEN);
				
				memcpy(NB_NetPar.ServerApn, 
								p_solid_data_struct->solid_data_cell_struct.solid_data_cell_data_struct.ee_addr_apn, 
								SERVER_APN_LEN);
		}
		else
		{
		#if SEGGER_RTT_DEBUG_FLASH
				SEGGER_RTT_printf(0, "init solid flash data!\r\n");
		#endif
//				NRF_LOG_DIRECT("init solid rom data!\r\n");
//				init_register_data();
//				init_param_data();
//				init_aes_key_data();
//				Set_Task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
		}
}

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

//        pg_num_a = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_A_PAGE;  // Use last 4 page in flash 页数量
//        pg_num_b = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_B_PAGE;  // Use last 3 page in flash 页数量

//		Read_Romdata(pg_num_a, p_dynamic_data_a_struct);
//		Read_Romdata(pg_num_b, p_dynamic_data_b_struct);

//		//只有倒数第4块flash存有有效数据
//		if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			   &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 != 0xaa)))
//		{
//				p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_a_struct->dynamic_data_cell_struct.num+1;
//				return store_dynamic_flash(pg_num_b, p_dynamic_data_struct);
//		}
//		//只有倒数第3块flash存有有效数据
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 != 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				p_dynamic_data_struct->dynamic_data_cell_struct.num = p_dynamic_data_b_struct->dynamic_data_cell_struct.num+1;
//				return store_dynamic_flash(pg_num_a, p_dynamic_data_struct);
//		}
//		//两块flash都存有有效数据
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

//    pg_num_a = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_A_PAGE;  // Use last 4 page in flash 页数量
//    pg_num_b = NRF_FICR->CODESIZE - DYNAMIC_ROM_DATA_B_PAGE;  // Use last 3 page in flash 页数量

//		Read_Romdata(pg_num_a,p_dynamic_data_a_struct);
//		Read_Romdata(pg_num_b,p_dynamic_data_b_struct);

//		//只有倒数第4块flash存有有效数据
//		if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 == 0xaa))
//			   &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 != 0xaa)))
//		{
//				get_dynamic_one_flash(p_dynamic_data_a_struct);
//		}
//		//只有倒数第3块flash存有有效数据
//		else if(((p_dynamic_data_a_struct->dynamic_data_cell_struct.writed1 != 0xaa)&&(p_dynamic_data_a_struct->dynamic_data_cell_struct.writed2 != 0xaa))
//			      &&((p_dynamic_data_b_struct->dynamic_data_cell_struct.writed1 == 0xaa)||(p_dynamic_data_b_struct->dynamic_data_cell_struct.writed2 == 0xaa)))
//		{
//				get_dynamic_one_flash(p_dynamic_data_b_struct);
//		}
//		//两块flash都存有有效数据
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
//		//两块flash都没存有效数据
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

