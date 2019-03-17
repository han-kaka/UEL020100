#include "task_mem_write.h"

uint8_t MEM_Write_Task(uint8_t prio)
{   
//		SEGGER_RTT_printf(0, "MEM_Write_Task\r\n");
	
    uint8_t m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio]) 
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        { 
            case MEM_STORE_SOLID_ROMDATA:   
            {
								store_solid_flsh_data();
            }
                break;  
            case MEM_STORE_DYNAMIC_ROMDATA:   
            {
//								Store_Dynamic_Data();
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
                                           
