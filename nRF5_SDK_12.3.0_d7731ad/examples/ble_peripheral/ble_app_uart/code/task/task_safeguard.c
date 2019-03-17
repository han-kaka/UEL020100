#include "task_safeguard.h"


uint8_t SG_Task(uint8_t prio)
{
//		SEGGER_RTT_printf(0, "SG_Task\r\n");
	
    uint8_t m_SYS_SubTask_prio=0;
	
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
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

