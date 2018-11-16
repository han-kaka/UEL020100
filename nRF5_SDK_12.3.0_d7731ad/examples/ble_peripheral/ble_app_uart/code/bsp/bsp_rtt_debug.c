#include "bsp_rtt_debug.h"


void ble_command_rx_log(uint16_t _length,uint8_t *_data_encrypt)
{
		SEGGER_RTT_printf(0,"data length = %d\r\n", _length);
	
		for(int i = 0; i < _length; i++)
		{
				SEGGER_RTT_printf(0, "data_encrypt[%d] = 0x%02X\r\n",i,_data_encrypt[i]);
		}
//  for(int i = 0;i < _length;i++)
//	{
//		SEGGER_RTT_printf(0, "data_decode[%d] = 0x%02X\r\n",i,_data_decode[i]);
//	}
}


