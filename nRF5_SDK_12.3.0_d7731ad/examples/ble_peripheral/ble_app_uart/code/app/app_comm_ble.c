#include "app_comm_ble.h"
#include "nrf_drv_rng.h"
#include "aes.h"

//BLE_Handler_t        g_stBLE_Handler;

uint8_t              char4_all_send_flag = 0;//用于数据发送
uint8_t              char4_all_send_len = 0;//用于数据发送
uint8_t              char4_all_send[256];//用于数据发送

static uint8_t f_tempkey = 0; /* 更新秘钥标志位 */

uint8_t *pRecvBuffer;
uint8_t stateProcessCommand = 0;

uint8_t log_index[LOG_INDEX_LEN];

uint8_t const CRC_TABLE[256] =
{
	0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41,
	0x9D,0xC3,0x21,0x7F,0xFC,0xA2,0x40,0x1E,0x5F,0x01,0xE3,0xBD,0x3E,0x60,0x82,0xDC,
	0x23,0x07,0x9F,0xC1,0x42,0x1C,0xFE,0xA0,0xE1,0xBF,0x5D,0x03,0x80,0xDE,0x3C,0x62,
	0xBE,0xE0,0x02,0x5C,0xDF,0x81,0x63,0x3D,0x7C,0x22,0xC0,0x9E,0x1D,0x43,0xA1,0xFF,
	0x46,0x18,0xFA,0xA4,0x27,0x79,0x9B,0xC5,0x84,0xDA,0x38,0x66,0xE5,0xBB,0x59,0x07,
	0xDB,0x85,0x67,0x39,0xBA,0xE4,0x06,0x58,0x19,0x47,0xA5,0xFB,0x78,0x26,0xC4,0x9A,
	0x65,0x3B,0xD9,0x87,0x04,0x5A,0xB8,0xE6,0xA7,0xF9,0x1B,0x45,0xC6,0x98,0x7A,0x24,
	0xF8,0xA6,0x44,0x1A,0x99,0xC7,0x25,0x7B,0x3A,0x64,0x86,0xD8,0x5B,0x05,0xE7,0xB9,
	0x8C,0xD2,0x30,0x6E,0xED,0xB3,0x51,0x0F,0x4E,0x10,0xF2,0xAC,0x2F,0x71,0x93,0xCD,
	0x11,0x4F,0xAD,0xF3,0x70,0x2E,0xCC,0x92,0xD3,0x8D,0x6F,0x31,0xB2,0xEC,0x0E,0x50,
	0xAF,0xF1,0x13,0x4D,0xCE,0x90,0x72,0x2C,0x6D,0x33,0xD1,0x8F,0x0C,0x52,0xB0,0xEE,
	0x32,0x6C,0x8E,0xD0,0x53,0x0D,0xEF,0xB1,0xF0,0xAE,0x4C,0x12,0x91,0xCF,0x2D,0x73,
	0xCA,0x94,0x76,0x28,0xAB,0xF5,0x17,0x49,0x08,0x56,0xB4,0xEA,0x69,0x37,0xD5,0x8B,
	0x57,0x09,0xEB,0xB5,0x36,0x68,0x8A,0xD4,0x95,0xCB,0x29,0x77,0xF4,0xAA,0x48,0x16,
	0xE9,0xB7,0x55,0x0B,0x88,0xD6,0x34,0x6A,0x2B,0x75,0x97,0xC9,0x4A,0x14,0xF6,0xA8,
	0x74,0x2A,0xC8,0x96,0x15,0x4B,0xA9,0xF7,0xB6,0xE8,0x0A,0x54,0xD7,0x89,0x6B,0x35,
};

uint8_t CRC_8(uint8_t *pdata, uint16_t size)
{
		uint8_t  CRC_result = 0;

		while (size--)
		{
			CRC_result ^= *pdata ++;
			CRC_result = CRC_TABLE[CRC_result];
		}
		return CRC_result;
}

uint8_t UserParseAppProtocolFormat(uint8_t * pBuf, uint16_t length)
{
		uint8_t  *p = pBuf;
		uint16_t tmp16;
		
//		if (gLimitALL != 0xA5) return 0;
		pRecvBuffer = 0;															                              // 初始化变量
//		if ((Get_SysRunState() != 1) && (Get_SysRunState() != 2))					          // 判断系统运行状态
//		{
//				return 0;
//		}
	
		tmp16 = *p++;																                                // 获取协议头
		tmp16 <<= 8;
		tmp16 |= *p++;
		if (tmp16 != PROTOCOL_APP_HEAD)												                      // 判断协议头正确性
		{
				return 0;
		}
		sProtocolAppFormat.headData.version = *p++;						                      // 获取协议中版本
		sProtocolAppFormat.headData.version <<= 8;
		sProtocolAppFormat.headData.version |= *p++;
		sProtocolAppFormat.headData.appScenarios = *p++;			                      // 获取协议中场景
		sProtocolAppFormat.headData.vendorCode = *p++;
		sProtocolAppFormat.headData.res[0] = *p++;						                      // 获取协议中保留字节
		sProtocolAppFormat.headData.res[1] = *p++;						                      // 获取协议中保留字节
		sProtocolAppFormat.headData.res[2] = *p++;						                      // 获取协议中保留字节
		sProtocolAppFormat.headData.command = *p++;						                      // 获取协议中命令
		sProtocolAppFormat.headData.dataSources = *p++;				                      // 获协议中数据来源	
		if ((sProtocolAppFormat.headData.dataSources != PROTOCOL_APP_SRC_MPHONE) && 
				(sProtocolAppFormat.headData.dataSources != PROTOCOL_APP_SRC_GATEWAY))	// 判断协议中数据来源
		{
				return 0;
		}		
		sProtocolAppFormat.headData.dataLen = *p++;									                // 获取协议中数据长度
		sProtocolAppFormat.pData = p;												                        // 获取数据开始位置
		p += sProtocolAppFormat.headData.dataLen;
		sProtocolAppFormat.tailData.crc8 = *p++;									                  // 获取协议中校验位
		tmp16 = *p++;																                                // 获取协议尾
		tmp16 <<= 8;
		tmp16 |= *p++;
		if (tmp16 != PROTOCOL_APP_TAIL)												                      // 获取协议尾正确性
		{
				return 0;
		}
		pRecvBuffer = pBuf;															                            // 获取协议总长度 
		
		return 1;
}

/**************************************************************************************************
 * @brief       检查数据包格式
 * @param       command：命令
 * @param       errCode：错误代码
 * @param       pData：数据
 * @param       len：数据长度
 * @return      0-失败；1-成功；
 **************************************************************************************************
 */
uint8_t UserCheckAppProtocolFormat(uint8_t *pBuf, uint16_t inputLen)					// 协议解析是否正确
{
		uint8_t  *p = pBuf;
		uint16_t  len;
	
		if ((p[0] != (uint8_t)(PROTOCOL_APP_HEAD >> 8)) || 
				(p[1] != (uint8_t)(PROTOCOL_APP_HEAD)))									// 判断协议头正确性
		{
				return 0;
		}
		len = p[11] + sizeof(ProtocolAppHeadFormat_t);								// 获取协议中数据长度
		if ((len + 3) > inputLen)
		{
				return 0;
		}
		if ((p[len + 1] != (uint8_t)(PROTOCOL_APP_TAIL >> 8)) || 
				(p[len + 2] != (uint8_t)(PROTOCOL_APP_TAIL)))								// 判断协议头正确性
		{
				return 0;
		}
		if (CRC_8(p, len) != p[len])
		{
				return 0;
		}
	
		return 1;
}



////void APP_BLETask_StateProc(void)
////{
////    switch(g_stBLE_Handler.State)
////    {
////				case BLE_STATE_CRC:
////        {
////						
////				}
////						break;
////				
////				case BLE_STATE_DECODE:
////        {
////				}
////						break;
////				
////				case BLE_STATE_ACTION:
////        {
////				}
////						break;
////				
////				case BLE_STATE_ENCRYPT:
////        {
////				}
////						break;
////				
////				case BLE_STATE_RETURN:
////        {
////				}
////						break;
////				
////        default:
////				{
////						g_stBLE_Handler.State = BLE_STATE_IDLE;
////				}
////						break;
////		}			
////	
////}

void Put_Return(uint8_t command, uint8_t length)
{
		uint8_t  *pBuf;
		uint16_t tmpLen, postion;
		
		postion = 0;
		pBuf = char4_all_send;
		pBuf[postion++] = HI_UINT16(PROTOCOL_APP_HEAD);
		pBuf[postion++] = LO_UINT16(PROTOCOL_APP_HEAD);
		pBuf[postion++] = HI_UINT16(PROTOCOL_APP_VERSION);
		pBuf[postion++] = LO_UINT16(PROTOCOL_APP_VERSION);
		pBuf[postion++] = PROTOCOL_APP_SCENARIOS;
		pBuf[postion++] = PROTOCOL_APP_VENDORCODE;
		pBuf[postion++] = PROTOCOL_APP_RESERVE;
		pBuf[postion++] = PROTOCOL_APP_RESERVE;
		pBuf[postion++] = PROTOCOL_APP_RESERVE;
		pBuf[postion++] = command;
		pBuf[postion++] = sProtocolAppFormat.headData.dataSources;
		if (((command == CMD_READ_LOG) || (command == CMD_ADD_USER)) && (length > 0))
		{
				tmpLen = AES_get_length(length - 1);
				pBuf[postion++] = tmpLen + 1;
				postion++;
				AES_Encrypt_PKCS7(pBuf + postion, pBuf + postion, length - 1, useraeskeybuf, useraeskeybuf); 
				postion += tmpLen;
				tmpLen = postion;
				pBuf[postion++] = CRC_8(pBuf, tmpLen);
				pBuf[postion++] = HI_UINT16(PROTOCOL_APP_TAIL);
				pBuf[postion++] = LO_UINT16(PROTOCOL_APP_TAIL);
		}
		else
		{
				pBuf[postion++] = length;
				postion += length;
				tmpLen = postion;
				pBuf[postion++] = CRC_8(pBuf, tmpLen);
				pBuf[postion++] = HI_UINT16(PROTOCOL_APP_TAIL);
				pBuf[postion++] = LO_UINT16(PROTOCOL_APP_TAIL);
		}
#if SEGGER_RTT_DEBUG_AES_DECODE
		SEGGER_RTT_printf(0, "postion = %d!\r\n", postion);
#endif
#if SEGGER_RTT_DEBUG_SEND_DATA	
		ble_command_rx_log(postion, pBuf, SEND_DATA);
#endif	
		bsp_ble_value_tx(pBuf, postion);

		if (f_tempkey)
		{
				f_tempkey = 0;
				memcpy(useraeskeybuf, tempkey, 16);
				set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
		}
}

/**************************************************************************************************
 * @brief       返回错误代码
 * @param       command：命令
 * @param       errCode：错误代码
 * @return      None
 **************************************************************************************************
 */
static void UserReturnErrCode(uint8_t command, uint8_t errCode)
{
		uint8_t  postion = 0;
		uint8_t  *pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
		
		pSendbuf[postion++] = errCode;
		Put_Return(command, postion);
}

/**************************************************************************************************
 * @brief       返回错误代码带数据
 * @param       command：命令
 * @param       errCode：错误代码
 * @param       pData：数据
 * @param       len：数据长度
 * @return      0-成功；other-失败；
 **************************************************************************************************
 */
static void UserReturnErrCodeAndData(uint8_t command, uint8_t errCode, char *pData, uint8_t len)
{
		uint8_t  postion = 0;
		uint8_t  *pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
		
		pSendbuf[postion++] = errCode;
		memcpy(pSendbuf + postion, pData, len);
		postion += len;
		Put_Return(command, postion++);
}

uint8_t ProcessCommand(uint8_t *pData, uint8_t command, uint16_t dataLen)
{
		uint8_t  i, tmp;
		uint8_t  ret = 0;
		uint8_t  *pSendbuf;
		uint16_t sendLength = 0;
		#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
		uint16 temp16;
		#endif
		uint32_t tmp32;
		static uint8_t  sReadUserCnt = 0;
		static uint8_t  sFlagAddUser = 0;
		uint8_t  buf[16];
		LogInfo_t *pTmpLogInfo = (LogInfo_t *)buf;
	
		if (sProtocolAppFormat.headData.version != PROTOCOL_APP_VERSION)			// 判断协议头正确性
		{
		#if SEGGER_RTT_DEBUG_AES_DECODE
				SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_VERSION_1!\r\n");
		#endif
				UserReturnErrCode(command, PROTOCOL_APP_ERR_VERSION);
				return ret;
		}
		if (dataLen == 0)
		{
		#if SEGGER_RTT_DEBUG_AES_DECODE
				SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_PARAM_2!\r\n");
		#endif
				UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
				return ret;
		}
		if ((gSystemRunParam.flagInit != 0xA5) && (command != CMD_GET_AESKEY))
		{
		#if SEGGER_RTT_DEBUG_AES_DECODE
				SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_PARAM_3!\r\n");
		#endif
				UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
				return ret;
		}
		#if SEGGER_RTT_DEBUG_AES_DECODE
				SEGGER_RTT_printf(0, "command = %02x\r\n",command);
		#endif
		
		switch (command)
		{
				case CMD_GET_HARD_INFO_VERSION:											/* 获取硬件版本信息 */
						UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, HARD_VERSION_INFO, sizeof(HARD_VERSION_INFO));
						break;
				
				case CMD_GET_SOFT_INFO_VERSION:											/* 查询软件版本信息 */
						UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, SOFT_VERSION_INFO, sizeof(SOFT_VERSION_INFO));
						break;
				
				case CMD_GET_AESKEY:													      /* 初始化 */
				{
				#if SEGGER_RTT_DEBUG_GET_AESKEY
						SEGGER_RTT_printf(0, "cmd get aeskey!\r\n");
				#endif
						
						if (*pData != 0x01)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						if(gSystemRunParam.flagInit == 0xA5) 
								break;
						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
						pSendbuf[sendLength++] = PROTOCOL_APP_ERR_NONE;
						nrf_drv_rng_rand(tempkey, 16); 
						memcpy(pSendbuf + sendLength, tempkey, 16);
						sendLength += 16;
						f_tempkey = 1;
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
				#if SEGGER_RTT_DEBUG_GET_AESKEY
						SEGGER_RTT_printf(0, "sendLength = %d!\r\n", sendLength);
				#endif
						Put_Return(command, sendLength);
						gSystemRunParam.flagInit = 0xA5;
				}
						break;
				
				case CMD_ADD_ADMIN:														/* 添加管理员 */
				case CMD_ADD_USER:														/* 添加用户 */
				{
				#if SEGGER_RTT_DEBUG_ADD_USER
						SEGGER_RTT_printf(0, "cmd add user!\r\n");
				#endif
					
						if (stateProcessCommand == 0)
						{
								if ((dataLen != 16) && (dataLen != 24))
								{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
								
								if (dataLen == 16) 
								{
										sFlagAddUser = 1;
										if (UserMemCmp(pData + 8, timestamp, 8) <= 0) break;
										memcpy(timestamp, pData+8, 8);
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								}
								else 
								{
										sFlagAddUser = 11;
										if (UserMemCmp(pData + 16, timestamp, 8) <= 0) break;
										memcpy(timestamp, pData+16, 8);
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								}
								stateProcessCommand = 1;
								ret = 0xFF;
						#if SEGGER_RTT_DEBUG_ADD_USER
								SEGGER_RTT_printf(0, "stateProcessCommand = %d\r\n", stateProcessCommand);
						#endif
						}
						else if (stateProcessCommand == 1)
						{
								ret = UserAddUserInfoToSystem(sFlagAddUser, 0, pData);
								if (ret == 0)
								{
										sReadUserCnt = 0;
										UserSetCurrentUser(pData);
										UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, (char *)pData + 8, 16);
								#if SEGGER_RTT_DEBUG_ADD_USER
										SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_NONE\r\n");
								#endif
								}
								else if (ret == 0xFF)
								{
								}
								else
								{
										UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_GEN, (char *)pData + 8, 16);
								#if SEGGER_RTT_DEBUG_ADD_USER
										SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_GEN\r\n");
								#endif
								}
						}
						else 
						{
								UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_GEN, (char *)pData + 8, 16);
						#if SEGGER_RTT_DEBUG_ADD_USER
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_GEN\r\n");
						#endif
						}
				}
						break;
				
				case CMD_READ_USER:														/* 读取用户 */
				{
				#if SEGGER_RTT_DEBUG_READ_USER
						SEGGER_RTT_printf(0, "cmd read user!\r\n");
				#endif
					
						if ((dataLen != 1) || (*pData != 0xA2))
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
						if (UserGetCurrentUser() == 0)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* 未登录，返回权限错误 */
						while (1)
						{
								Rom_Data_Type user_info_data_struct;
								p_Rom_Data_Type p_user_info_data_struct = &user_info_data_struct;	
								read_user_info_data(p_user_info_data_struct);
							
								if (sReadUserCnt < MAX_USER_NUM)
								{
										if (UserReadUserInfoUID(sReadUserCnt, buf, p_user_info_data_struct) == 0)
										{
												UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, (char *)buf, 8);
												sReadUserCnt++;
												break;
										}
										sReadUserCnt++;
								}
								else
								{
										UserReturnErrCode(command, PROTOCOL_APP_ERR_FINISHED);
										break;
								}
						}
				}
						break;
				
				case CMD_DEL_USER:														/* 删除用户 */
				{
				#if SEGGER_RTT_DEBUG_DEL_USER
						SEGGER_RTT_printf(0, "cmd del user!\r\n");
				#endif
						if (dataLen != 8)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
						if (UserGetCurrentUser() == 0)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* 未登录，返回权限错误 */
						
						UserDelUserInfoFromSystem(pData);
						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}	
					break; 
				
				case CMD_USER_LOGIN:													/* 用户登录 */
				{
				#if SEGGER_RTT_DEBUG_USER_LOGIN
						SEGGER_RTT_printf(0, "cmd user login!\r\n");
				#endif
						Rom_Data_Type user_info_data_struct;
						p_Rom_Data_Type p_user_info_data_struct = &user_info_data_struct;	
					
						if (dataLen != 16)
						{
						#if SEGGER_RTT_DEBUG_USER_LOGIN
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_PARAM\r\n");
						#endif
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						
						if (UserMemCmp(pData+8, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData+8, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
						
						read_user_info_data(p_user_info_data_struct);
						tmp = UserSearchUserInfoNumber(pData, p_user_info_data_struct);
						if (tmp < MAX_USER_NUM)
						{
								sReadUserCnt = 0;
								UserSetCurrentUser(pData);
						#if SEGGER_RTT_DEBUG_USER_LOGIN
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_NONE\r\n");
						#endif
								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
						}
						else
						{
						#if SEGGER_RTT_DEBUG_USER_LOGIN
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_GEN\r\n");
						#endif
								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
						}
				}
					break;
				
				case CMD_SET_KEYBOARD:													/* 设置键盘密码 */
				{
				#if SEGGER_RTT_DEBUG_SET_KEYBOARD
						SEGGER_RTT_printf(0, "cmd set keyboard!\r\n");
				#endif
						UserReturnErrCode(command, PROTOCOL_APP_ERR_VERSION);
				}
						break;
				
				case CMD_ADD_PERIPHERAL:												/* 添加外设 */
				{
				#if SEGGER_RTT_DEBUG_ADD_PERIPHERAL
						SEGGER_RTT_printf(0, "cmd add peripheral!\r\n");
				#endif			
						Rom_Data_Type user_info_data_struct;
						p_Rom_Data_Type p_user_info_data_struct = &user_info_data_struct;
					
						if (dataLen != 1)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
						if (UserGetCurrentUser() == 0)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* 未登录，返回权限错误 */
						read_user_info_data(p_user_info_data_struct);
						tmp = UserSearchUserInfoNumber(UserGetCurrentUser(), p_user_info_data_struct);
						if (tmp >= MAX_USER_NUM)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}
						if (*pData == 1)
						{
						#if (defined USER_FINGERMODE) && (!(defined USER_FINGERMODEVENA))
								if (UserFingerModuleAddition(tmp, UserFingerAdditionCallback) == FALSE)
								{
									UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
									break;
								}
						#else
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						#endif
						}
						else if (*pData == 2)
						{
						#ifdef USER_ICCARD
								if (UserICCardAddition(UserGetCurrentUser()) == FALSE)
								{
									UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
									break;
								}
						#else
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						#endif
						}
						else if (*pData == 4)
						{
						#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
								if (UserFingerModuleAddition(tmp, UserFingerAdditionCallback) == FALSE)
								{
									UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
									break;
								}
						#else
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						#endif
						}
						else
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
//								break;
						}
//						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}
						break;
					
				case CMD_SWITCH:														/* 开关指令 */
				{
						LogInfo_t tmpLogInfo;
					
						#if SEGGER_RTT_DEBUG_SWITCH
								SEGGER_RTT_printf(0, "cmd switch!\r\n");
						#endif
					
						if (dataLen != 9)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						
						if (UserMemCmp(pData, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
						
						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
						pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_NONE;
						gSystemRunParam.batterPercent = (gSystemRunParam.advc - 370)*100/50;
						pSendbuf[sendLength ++] = gSystemRunParam.batterPercent;
						tmp = 0;
						if (gFlagAdjustTime <= 1) 
						{
								tmp |= BV(0);
						}
						if (UserGetLogInfo(0) == 1) 
						{
								tmp |= BV(1);
						}
						pSendbuf[sendLength ++] = tmp;
						Put_Return(command, sendLength);
						
						tmpLogInfo.flag = 0xA5;
						tmpLogInfo.action = 0x04;
						tmpLogInfo.time = seconds_times;
						UserSearchUserInfoUID(gCurrentLocalUser, tmpLogInfo.userId);
						UserSaveLogInfo(&tmpLogInfo);
						
						if (pData[8] == 0xA1)
						{
								Motor_State_Struct = INVERSION;
								task_flag_struct.motordelay = ENABLE;
						}
						else if (pData[8] == 0xA2)
						{
								Motor_State_Struct = FOREWARD;
								task_flag_struct.motordelay = ENABLE;
						}
				}
						break;
				
				case CMD_ADJUST_TIME:													/* 校时指令 */
				{
						#if SEGGER_RTT_DEBUG_ADJUST_TIME
								SEGGER_RTT_printf(0, "cmd adjust time!\r\n");
						#endif
					
						#if PROTOCOL_APP_VERSION == 0x0100
			/****************************** V1.0 版本程序开始 *****************************/
						if (dataLen != 4)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
//						if (UserGetCurrentUser() == 0)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* 未登录，返回权限错误 */
						tmp32 = pData[0];
						tmp32 = (tmp32 << 8) | pData[1];
						tmp32 = (tmp32 << 8) | pData[2];
						tmp32 = (tmp32 << 8) | pData[3];
			/****************************** V1.0 版本程序结束 *****************************/
						#else
			/****************************** V1.1 版本程序开始 *****************************/
						if (dataLen != 12)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}			/* 数据长度错误，返回参数错误 */
						
						if (UserMemCmp(pData, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
						
						tmp32 = pData[8];
						tmp32 = (tmp32 << 8) | pData[9];
						tmp32 = (tmp32 << 8) | pData[10];
						tmp32 = (tmp32 << 8) | pData[11];
			/****************************** V1.1 版本程序结束 *****************************/
						#endif
						
						if (tmp32 < SECONDS2000YEAR)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						tmp32 -= SECONDS2000YEAR;
						seconds_times = tmp32;
						gFlagAdjustTime = 60000; /* 大约一周时间 */
						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
				}
						break;
				
				case CMD_READ_LOG:														/* 读取日志 */
				{
				#if SEGGER_RTT_DEBUG_READ_LOG
						SEGGER_RTT_printf(0, "cmd read log!\r\n");
				#endif
						pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
						if (UserGetLogInfo(pTmpLogInfo) == 1)
						{
						#if SEGGER_RTT_DEBUG_READ_LOG
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_NONE\r\n");
						#endif
								pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_NONE;						
								memcpy(pSendbuf + sendLength, pTmpLogInfo->userId, sizeof(pTmpLogInfo->userId));
								sendLength += sizeof(pTmpLogInfo->userId);
								pTmpLogInfo->time += SECONDS2000YEAR;
								pSendbuf[sendLength ++] = pTmpLogInfo->time >> 24;
								pSendbuf[sendLength ++] = pTmpLogInfo->time >> 16;
								pSendbuf[sendLength ++] = pTmpLogInfo->time >> 8;
								pSendbuf[sendLength ++] = pTmpLogInfo->time >> 0;
								pSendbuf[sendLength ++] = pTmpLogInfo->action;
						}
						else 
						{
						#if SEGGER_RTT_DEBUG_READ_LOG
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_FINISHED\r\n");
						#endif
								pSendbuf[sendLength ++] = PROTOCOL_APP_ERR_FINISHED;
								memset(pSendbuf + sendLength, 0, 13);
								sendLength += 13;
						}
						
						Put_Return(command, sendLength);
				}
						break;
			
				case CMD_READ_USER_CONFIG:												/* 读取用户信息 */
				{
				#if SEGGER_RTT_DEBUG_READ_USER
						SEGGER_RTT_printf(0, "cmd read log!\r\n");
				#endif
						if ((dataLen != 1) || (*pData != 0xAB))
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
						if (UserGetCurrentUser() == 0)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_PERMIT);break;}	/* 未登录，返回权限错误 */
						
						if (UserReadUserInfoConfig(UserGetCurrentUser(), &tmp) == 0)
						{
								buf[0] = 0; buf[1] = 0;
								if (tmp & 0x40) buf[1] |= 0x01;
								if (tmp & 0x20) buf[1] |= 0x02;
								if (tmp & 0x10) buf[1] |= 0x04;
								if (tmp & 0x08) buf[1] |= 0x08;
								UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, (char *)buf, 2);
						}
						
						while (1)
						{
								Rom_Data_Type user_info_data_struct;
								p_Rom_Data_Type p_user_info_data_struct = &user_info_data_struct;	
								read_user_info_data(p_user_info_data_struct);
							
								if (sReadUserCnt < MAX_USER_NUM)
								{
										if (UserReadUserInfoUID(sReadUserCnt, buf, p_user_info_data_struct) == 0)
										{
												UserReturnErrCodeAndData(command, PROTOCOL_APP_ERR_NONE, (char *)buf, 8);
												sReadUserCnt++;
												break;
										}
										sReadUserCnt++;
								}
								else
								{
										UserReturnErrCode(command, PROTOCOL_APP_ERR_FINISHED);
										break;
								}
						}
				}
						break;
				
				case CMD_DEL_LOG:														/* 删除日志 */
				{
				#if SEGGER_RTT_DEBUG_DEL_LOG
						SEGGER_RTT_printf(0, "cmd del log!\r\n");
				#endif
//						if (dataLen != 16)
//						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}			/* 数据长度错误，返回参数错误 */
						memcpy(pTmpLogInfo->userId, pData, sizeof(pTmpLogInfo->userId));
						tmp32 = pData[sizeof(pTmpLogInfo->userId) + 0];
						tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 1];
						tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 2];
						tmp32 = (tmp32 << 8) | pData[sizeof(pTmpLogInfo->userId) + 3];
						pTmpLogInfo->time = tmp32 - SECONDS2000YEAR;
						if (UserDelLogInfo(pTmpLogInfo) == 1)
						{
						#if SEGGER_RTT_DEBUG_READ_LOG
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_NONE\r\n");
						#endif
								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
						}
						else 
						{
						#if SEGGER_RTT_DEBUG_READ_LOG
								SEGGER_RTT_printf(0, "PROTOCOL_APP_ERR_GEN\r\n");
						#endif
								UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
						}
				}
						break;
					
				case CMD_FACTORYRESET:													/* 恢复出厂设置 */
				{
				#if SEGGER_RTT_DEBUG_FACTORYRESET
						SEGGER_RTT_printf(0, "cmd factoryrest!\r\n");
				#endif
						if (dataLen != 129)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
						if (pData[8] != 0xAA)
						{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}					
						if (UserMemCmp(pData, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
						UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
						
//						UserClearUserInfoFromSystem();
//						gSystemRunParam.flagInit = 0x00;									// 清除管理员及密码
//						SaveSetup();														// 保存数据
//						Save_SysRunState(1);												// 添加管理员模式
//						WaitSystemExit(0, 20000);
//						ChangescanRspData(sizeof ( lockinitname ), (uint8*)lockinitname);	// 初始化广播名字
//						UserSaveAppData(P_EE_ADDR_LOCKNAME, (uint8*)lockinitname);			// update to EEPROM
//						UserStartAdv(1, 0);													// 立即开始广播
				}
						break;
				
				case CMD_TERFILEDATA:													/* 写设备文件数据 */
				{
				#if SEGGER_RTT_DEBUG_TERFILEDATA
						SEGGER_RTT_printf(0, "cmd terfiledata!\r\n");
				#endif
				#if (defined USER_FINGERMODE) && (defined USER_FINGERMODEVENA)
//						temp16 = pData[0];
//						temp16 = (temp16 << 8) | pData[1]; /* 读取包序号 */
//						if (temp16 == 0) /* 文件信息 */
//						{
//							if (pData[2] != 1)  /* 判断指纹数据 */
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//							tmp32 = pData[3];
//							tmp32 = (tmp32 << 8) | pData[4];
//							tmp32 = (tmp32 << 8) | pData[5];
//							tmp32 = (tmp32 << 8) | pData[6]; /* 读取文件长度 */
//							sTransferFile.dataLength = tmp32;
//							tmp32 = pData[7];
//							tmp32 = (tmp32 << 8) | pData[8];
//							tmp32 = (tmp32 << 8) | pData[9];
//							tmp32 = (tmp32 << 8) | pData[10]; /* 读取文件累加和 */
//							sTransferFile.dataSum = tmp32;
//							
//							tmp = UserSearchUserInfoNumber(pData + 11);
//							if (tmp >= MAX_USER_NUM)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN); break;}
//							
//							UserGetAppData(P_EE_ADDR_TIMESTAMP, buf);
//			//				if (UserMemCmp(pData + 19, buf, 8) <= 0) break;
//							UserSaveAppData(P_EE_ADDR_TIMESTAMP, pData + 19);
//							
//							UserFingerModuleAddUserDataStart(tmp, sTransferFile.dataLength, UserTransferFileCallback);
//						}
//						else if (temp16 == 0xFFFF) /* 传输完成 */
//						{
//							UserFingerModuleAddUserDataEnd(0xFFFF, sTransferFile.dataSum);
//						}
//						else /* 文件数据 */
//						{
//							if (sTransferFile.currentNum != temp16)
//							{UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);break;}
//							UserFingerModuleAddUserDataIng(temp16, pData + 2);
//						}
				#else
						UserReturnErrCode(command, PROTOCOL_APP_ERR_GEN);
				#endif
				}
						break;
				
				case CMD_PARAM_CONFIG:													/* 参数配置指令 */
				{
					
					
						if (dataLen < 9)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						
						if (UserMemCmp(pData, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
						
						if (pData[8] == 0x01)	// 配置开锁方向
						{
								if ((pData[9] == 0x01) || pData[9] == 0x02)
								{
										memcpy(&open_lock_dir, pData+9, 1);
										set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
										UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
//										SC_ProtocolOpenLockDir(pData[9]);
								}
								else
								{
										UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								}
						}
						else if (pData[8] == 0x10)	// 配置IP地址和端口号
						{
								memcpy(NB_NetPar.ServerIP, pData+9, SERVER_IP_LEN);
								set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
						}
						else if (pData[8] == 0x11)	// 配置APN
						{
								memcpy(NB_NetPar.ServerAPN, pData+9, SERVER_APN_LEN);
								set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
								UserReturnErrCode(command, PROTOCOL_APP_ERR_NONE);
						}
						else
						{
							UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
						}
				}
						break;
					
				case CMD_PARAM_READ:													/* 参数读取指令 */
				{
					
						if (dataLen < 9)
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
								break;
						}
						if (UserMemCmp(pData, timestamp, 8) <= 0) 
								break;
						memcpy(timestamp, pData, 8);
						set_task(MEM_WRITE, MEM_STORE_SOLID_ROMDATA);
					
						if (pData[8] == 0x12)	// 参数IMEI
						{
								if (gSystemRunParam.flagAny & 0x01)
								{
										i = 0;
										pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
										pSendbuf[i++] = PROTOCOL_APP_ERR_NONE;		
										memcpy((uint8_t *)(pSendbuf + i), (uint8_t *)NB_CommPacket.Init_Data.imei, IMEI_LEN);
										tmp = SC_StrPrintlen(pSendbuf + i) % 17;
										pSendbuf[i + tmp] = 0; i ++;
										i += tmp;
										Put_Return(command, i);
								}
								else
								{
										UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_FOUND_NB);
								}
						}
						else if (pData[8] == 0x13)	// ICCID
						{
								if (gSystemRunParam.flagAny & 0x02)
								{
										i = 0;
										pSendbuf = char4_all_send + sizeof(ProtocolAppHeadFormat_t);
										pSendbuf[i++] = PROTOCOL_APP_ERR_NONE;
										memcpy((uint8_t *)(pSendbuf + i), (uint8_t *)NB_CommPacket.Init_Data.iccid, ICCID_LEN);									
										tmp = SC_StrPrintlen(pSendbuf + i) % 33;
										pSendbuf[i + tmp] = 0; i++;
										i += tmp;
										Put_Return(command, i);
								}
								else
								{
										UserReturnErrCode(command, PROTOCOL_APP_ERR_NOT_FOUND_SIM);
								}
						}
						else
						{
								UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
						}
				}
						break;
				
				default:
						UserReturnErrCode(command, PROTOCOL_APP_ERR_PARAM);
						break;
		}
		
		if (ret == 0xFF)
		{
			
		}
		else
		{
				stateProcessCommand = 0;
		}
		
		return ret;
}


