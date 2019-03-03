#include "bsp_rtt_debug.h"

void ble_command_rx_log(uint8_t _length, uint8_t *_data_encrypt, BLE_Data_Type data_type)
{
		switch(data_type)
		{
				case ORIGINAL_DATA:
				{
						SEGGER_RTT_printf(0,"original data length = %d. data = \r\n", _length);
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", _data_encrypt[i]);
						}
				}
						break;
			
				case PARSE_PROTOCOL:
				{
						SEGGER_RTT_printf(0,"data encrypt data length = %d. data = \r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", _data_encrypt[i]);
						}
				}
						break;
				
				case CRC_CHECK:
				{
						SEGGER_RTT_printf(0,"crc check data length = %d. data = \r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", _data_encrypt[i]);
						}
				}
						break;
				
				case AES_DECODE:
				{
						SEGGER_RTT_printf(0,"aes data length = %d. data = \r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", _data_encrypt[i]);
						}
						
						SEGGER_RTT_printf(0,"\naes key = \r\n");
					
						for(int i = 0; i < 16; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", useraeskeybuf[i]);
						}
				}
						break;

				case SEND_DATA:
				{
						SEGGER_RTT_printf(0,"send data length = %d. data = \r\n", _length);
					
						for(int i = 0; i < _length; i++)
						{
								SEGGER_RTT_printf(0, "%02x ", _data_encrypt[i]);
						}
				}
						break;
					
				default:
						break;
		}
		SEGGER_RTT_printf(0, "\r\n");
}


