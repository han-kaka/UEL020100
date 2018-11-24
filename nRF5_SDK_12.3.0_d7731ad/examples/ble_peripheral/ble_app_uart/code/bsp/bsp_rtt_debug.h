#ifndef __BSP_RTT_DEBUG_H
#define __BSP_RTT_DEBUG_H

#include "global.h"

typedef enum
{
		ORIGINAL_DATA=0,
		PARSE_PROTOCOL,
		CRC_CHECK,
		AES_DECODE,
	
}BLE_Data_Type;

void ble_command_rx_log(uint8_t _length, uint8_t *_data_encrypt, BLE_Data_Type data_type);

#endif


