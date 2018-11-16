#include "task_mem_read.h"

uint8_t MEM_Read_Task(uint8_t prio) 
{
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

