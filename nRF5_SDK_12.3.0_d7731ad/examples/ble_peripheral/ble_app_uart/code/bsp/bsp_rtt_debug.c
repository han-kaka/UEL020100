#include "bsp_rtt_debug.h"


void ble_command_rx_log(uint8_t _length, uint8_t *_data_encrypt, BLE_Data_Type data_type)
{
		switch(data_type)
		{
				case ORIGINAL_DATA:
				{
						SEGGER_RTT_printf(0,"original data length = %d\r\n", _length);
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "[%d] = 0x%02X\r\n",i,_data_encrypt[i]);
						}
				}
						break;
			
				case PARSE_PROTOCOL:
				{
						SEGGER_RTT_printf(0,"data encrypt data length = %d\r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "[%d] = 0x%02X\r\n",i,_data_encrypt[i]);
						}
				}
						break;
				
				case CRC_CHECK:
				{
						SEGGER_RTT_printf(0,"crc check data length = %d\r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "[%d] = 0x%02X\r\n",i,_data_encrypt[i]);
						}
				}
						break;
				
				case AES_DECODE:
				{
						SEGGER_RTT_printf(0,"aes data length = %d\r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "[%d] = 0x%02X\r\n",i,_data_encrypt[i]);
						}
				}
						break;
				
				default:
						break;
		}
}


