#include "task_bluetooth.h"

uint8_t BlueTooth_Task(uint8_t prio)
{
		uint8_t m_SYS_SubTask_prio=0;
		uint8_t  temp;
//		uint8_t i;
		while(ga_Subtask[prio])
		{
				m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
				switch(m_SYS_SubTask_prio)
				{
						case BLUETOOTH_PARSE_PROTOCOL:
						{
								if(UserParseAppProtocolFormat(char2_all_receive, char2_all_receive_len) == 1)
								{
								#if SEGGER_RTT_DEBUG_PARSE_PROTOCOL	
										ble_command_rx_log(char2_all_receive_len, char2_all_receive, PARSE_PROTOCOL);
								#endif	
										set_task(BLUETOOTH, BLUETOOTH_CRC_CHECK);
								}
								char2_all_receive_len = 0;
						}
								break;
					
						case BLUETOOTH_CRC_CHECK:
						{
								if (CRC_8(pRecvBuffer, sProtocolAppFormat.headData.dataLen + sizeof(ProtocolAppHeadFormat_t)) == sProtocolAppFormat.tailData.crc8)
								{
								#if SEGGER_RTT_DEBUG_CRC_CHECK	
										ble_command_rx_log(sProtocolAppFormat.headData.dataLen + sizeof(ProtocolAppHeadFormat_t) + 3, pRecvBuffer, CRC_CHECK);
								#endif	
										set_task(BLUETOOTH, BLUETOOTH_AES_DECODE);
								}
//								else 
//								{
//										return false;
//								}
						}
								break;
						
						case BLUETOOTH_AES_DECODE:
						{							
								sProtocolAppFormat.headData.dataLen = AES_Decrypt(sProtocolAppFormat.pData, sProtocolAppFormat.pData, sProtocolAppFormat.headData.dataLen, useraeskeybuf, useraeskeybuf);
								stateProcessCommand = 0;
						#if SEGGER_RTT_DEBUG_AES_DECODE	
								ble_command_rx_log(sProtocolAppFormat.headData.dataLen, sProtocolAppFormat.pData, AES_DECODE);
						#endif	
								set_task(BLUETOOTH, BLUETOOTH_PROCESS_CMD);
						}	
								break;
						
						case BLUETOOTH_PROCESS_CMD:
						{
								temp = ProcessCommand(sProtocolAppFormat.pData, sProtocolAppFormat.headData.command, sProtocolAppFormat.headData.dataLen);
								if(temp == 0xFF)
								{
										return false;
								}
						}	
								break;			
						
						default:
								break;
				}
				if(clear_task(prio,m_SYS_SubTask_prio) == true)
				{
						return true;
				}
		}
		return false;
}
  

