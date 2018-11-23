#include "task_bluetooth.h"

uint8_t BlueTooth_Task(uint8_t prio)
{
		uint8_t m_SYS_SubTask_prio=0;
		while(ga_Subtask[prio])
		{
				m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
				switch(m_SYS_SubTask_prio)
				{
						case BLUETOOTH_PARSE_PROTOCOL:
						{
								if(UserParseAppProtocolFormat(data_encrypt, sizeof(data_encrypt)) == 1)
								{
										Set_Task(BLUETOOTH, BLUETOOTH_CRC_CHECK);
								}
						}
								break;
					
						case BLUETOOTH_CRC_CHECK:
						{
								if (CRC_8(pRecvBuffer, sProtocolAppFormat.headData.dataLen + sizeof(ProtocolAppHeadFormat_t)) == sProtocolAppFormat.tailData.crc8)
								{
										Set_Task(BLUETOOTH, BLUETOOTH_AES_DECODE);
								}
//								else 
//								{
//										return false;
//								}
						}
								break;
						
						case BLUETOOTH_AES_DECODE:
						{
								
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
  

