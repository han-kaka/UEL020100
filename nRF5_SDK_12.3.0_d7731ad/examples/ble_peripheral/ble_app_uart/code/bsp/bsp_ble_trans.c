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

uint8_t              char2_all_receive[256];
uint8_t              char2_all_receive_len = 0;
uint8_t              Char2_Once_Receive_Len = 0;

/* 
 * @brief function for ble data command receive
 *
 * @param p_data: pointer to rx data
 * @param length: ble rx data length
 */
void bsp_ble_command_rx(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
		if ((char2_all_receive_len + length) <= 128)
		{
				memcpy(&char2_all_receive[char2_all_receive_len], p_data, length);
				char2_all_receive_len += length;
		}

		Char2_Once_Receive_Len = length;
		
		if(UserCheckAppProtocolFormat(char2_all_receive, char2_all_receive_len) == 1)
		{
		#if SEGGER_RTT_DEBUG_ORIGINAL_DATA	
				ble_command_rx_log(char2_all_receive_len, char2_all_receive, ORIGINAL_DATA);
		#endif	
				Set_Task(BLUETOOTH, BLUETOOTH_PARSE_PROTOCOL);
		}
}

//void bsp_ble_command_explain(uint8_t * p_data, uint16_t length)
//{
//		UserParseAppProtocolFormat(p_data, length);
//		
//		if (CRC_8(pRecvBuffer, sProtocolAppFormat.headData.dataLen + sizeof(ProtocolAppHeadFormat_t)) != sProtocolAppFormat.tailData.crc8)
//		{
//				return;
//		}
//		
//		AES128_ECB_decrypt(sProtocolAppFormat.pData, useraeskeybuf, sProtocolAppFormat.pData);
//		
//		ProcessCommand(sProtocolAppFormat.pData, sProtocolAppFormat.headData.command, sProtocolAppFormat.headData.dataLen);
//}




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
