#include "bsp_ble_trans.h"
//#include "lpd_bsp_ble_cmdexplain.h"
//#include "nrf_log.h"
#include "aes.h"
//#include "ble_nus.h"
//#include "lpd_bsp_rtt_debug.h"

//extern ble_nus_t             m_nus; 

//bool ble_data_new = false;

ProtocolAppFormat_t sProtocolAppFormat;

uint8_t defualtaes128key[16] = {0x98, 0x76, 0x23, 0xE8, 0xA9, 0x23, 0xA1, 0xBB, 0x3D, 0x9E, 0x7D, 0x03, 0x78, 0x12, 0x45, 0x88};
uint8_t tempkey[16] = {0x98, 0x76, 0x23, 0xE8, 0xA9, 0x23, 0xA1, 0xBB, 0x3D, 0x9E, 0x7D, 0x03, 0x78, 0x12, 0x45, 0x88};
uint8_t useraeskeybuf[16] = {0x98, 0x76, 0x23, 0xE8, 0xA9, 0x23, 0xA1, 0xBB, 0x3D, 0x9E, 0x7D, 0x03, 0x78, 0x12, 0x45, 0x88};

uint8_t              data_encrypt[16];

static uint8_t *pRecvBuffer;

unsigned char const CRC_TABLE[];

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



static uint8_t CRC_8(uint8_t *pdata, uint16_t size)
{
		uint8_t  CRC_result = 0;

		while (size--)
		{
			CRC_result ^= *pdata ++;
			CRC_result = CRC_TABLE[CRC_result];
		}
		return CRC_result;
}

/* 
 * @brief function for ble data command receive
 *
 * @param p_data: pointer to rx data
 * @param length: ble rx data length
 */
void bsp_ble_command_rx(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{   
		memcpy(data_encrypt, p_data, sizeof(data_encrypt)); 
		
//		if(0 == UserCheckAppProtocolFormat(data_encrypt, sizeof(data_encrypt)))
//		{
//				AES128_ECB_decrypt(data_encrypt, useraeskeybuf, data_decode);
//		bsp_ble_command_explain(data_decode,sizeof(data_decode));
		Set_Task(BLUETOOTH, BLUETOOTH_DECODE);
		#if SEGGER_RTT_DEBUG_BLUETOOTH
				ble_command_rx_log(length, data_encrypt);
		#endif
//		}
}

void bsp_ble_command_explain(uint8_t * p_data, uint16_t length)
{
		UserParseAppProtocolFormat(p_data, length);
		
		if (CRC_8(pRecvBuffer, sProtocolAppFormat.headData.dataLen + sizeof(ProtocolAppHeadFormat_t)) != sProtocolAppFormat.tailData.crc8)
		{
				return;
		}
		
		AES128_ECB_decrypt(sProtocolAppFormat.pData, useraeskeybuf, sProtocolAppFormat.pData);
		
		ProcessCommand(sProtocolAppFormat.pData, sProtocolAppFormat.headData.command, sProtocolAppFormat.headData.dataLen);
}


uint8_t UserParseAppProtocolFormat(uint8_t * data_encrypt, uint16_t length)
{
		uint8_t  *p = data_encrypt;
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
		pRecvBuffer = data_encrypt;															                    // 获取协议总长度 
		
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

///* 
// * @brief function for ble data send
// *
// * @param p_data: pointer to tx data
// * @param length: ble tx data length
// *
// * @return sdk error info
// */
//uint32_t bsp_ble_value_tx(uint8_t * p_data, uint16_t length)
//{
//		return ble_nus_string_send(&m_nus,p_data,length);
//}

///* 
// * @brief ble handle task
// */
//void ble_process_task(void)
//{
//    if(ble_data_new == true)
//    {
//        bsp_ble_command_explain(data_decode,sizeof(data_decode));
//        ble_data_new = false;
//    }
//}
