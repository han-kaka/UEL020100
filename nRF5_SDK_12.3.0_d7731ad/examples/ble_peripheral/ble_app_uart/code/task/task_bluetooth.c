#include "task_bluetooth.h"

uint8_t BlueTooth_Task(uint8_t prio)
{
		uint8_t m_SYS_SubTask_prio=0;
		while(ga_Subtask[prio])
		{
				m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
				switch(m_SYS_SubTask_prio)
				{
						case BLUETOOTH_DECODE:
						{
								bsp_ble_command_explain(data_encrypt, sizeof(data_encrypt));
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
  

